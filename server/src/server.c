#include "server.h"
#include "libs/cthreads.h"
#include "libs/d_array.h"
#include "packets.h"
#include <stdint.h>
#include <time.h>

int server_init(Server* server) {
    if (tcs_lib_init() != TCS_SUCCESS) {
        perror("Could not init tcs\n");
        return 1;
    }

    printf("[INFO] Starting Server\n");

    server->running = true;

    server->server_socket = TCS_NULLSOCKET;
    if (tcs_create(&server->server_socket, TCS_TYPE_TCP_IP4) != TCS_SUCCESS) {
        perror("Could not create tcs socket\n");
        return 1;
    }

    if (tcs_listen_to(server->server_socket, PORT) != TCS_SUCCESS) {
        perror("Could not bind and listen to socket\n");
        return 1;
    }
    
    printf("[INFO] Listening on Port %d ...\n", PORT);

    return 0;
}

void server_run(Server* server) {
    while (server->running) {
        TcsSocket client_socket = TCS_NULLSOCKET;
        tcs_accept(server->server_socket, &client_socket, NULL);

        server->threads[server->client_count] = (ClientThreadCollection) {0};
        server->threads_data[server->client_count] = (ThreadData) {
            .server = server,
            .client_socket = client_socket,
            .other_threads = &server->threads[server->client_count],
            .send_data = (SendData) {
                .send_buffer = d_array_new(sizeof(uint8_t)),
                .new_send_data = false,
            },
        };

        if (cthreads_mutex_init(&server->threads_data[server->client_count].send_data.send_lock, NULL) != 0) {
            printf("[Error] could not create mutex\n");
            continue;
        }

        // create recv thread
        if (cthreads_thread_create(&server->threads[server->client_count].recv_thread,
                                   NULL,
                                   server_handle_client_recv, 
                                   (void*) &server->threads_data[server->client_count], 
                                   NULL) != 0) {
            printf("[ERROR] An Error Occured While Creating Thread!\n");
            // if cthreads_thread_create fails we can just continue because 
            // we didnt increment client_count yet
            continue;
        }

        // create send thread
        if (cthreads_thread_create(&server->threads[server->client_count].send_thread,
                                   NULL,
                                   server_handle_client_send, 
                                   (void*) &server->threads_data[server->client_count], 
                                   NULL) != 0) {
            printf("[ERROR] An Error Occured While Creating Thread!\n");
            // if cthreads_thread_create fails we can just continue because 
            // we didnt increment client_count yet
            continue;
        }
        
        server_send_ping_packet(&server->threads_data[server->client_count]);
        
        server->client_count ++;

    }

    for (size_t i = 0; i < server->client_count; ++i) {
        cthreads_thread_join(server->threads[i].send_thread, NULL);
        cthreads_thread_join(server->threads[i].recv_thread, NULL);
    }
}

void* server_handle_client_recv(void* data) {
    ThreadData* thread_data = (ThreadData*) data;

    // ClientThreadCollection* other_threads = thread_data->other_threads;

    for (;;) {
        PacketHeader header;
        thread_recv_header(thread_data, &header);
        uint8_t recv_buffer[header.length];
        thread_recv_into_buffer(thread_data, recv_buffer, header.length);
        // now do the handle thing

        thread_handle_packet(thread_data, &header, recv_buffer, header.length);

    }
    return NULL;
}

void thread_recv_header(ThreadData* thread_data, PacketHeader* header) {
    uint8_t recv_buffer[sizeof(PacketHeader)];
    thread_recv_into_buffer(thread_data, recv_buffer, sizeof(recv_buffer));

    get_packet_header(recv_buffer, header);

    printf("[INFO] Deserialized Packet Header:\n");
    printf("    Type: %d\n", header->type);
    printf("    Length: %d\n", header->length);
}

void thread_recv_into_buffer(ThreadData* thread_data, uint8_t* buffer, size_t size) {
    tcs_receive(thread_data->client_socket, buffer, size, TCS_NO_FLAGS, NULL);
}

void thread_handle_packet(ThreadData* thread_data, PacketHeader* header, uint8_t* buffer, size_t size) {
     
    switch (header->type) {
        case PACKET_PING:
            break;
    }
}

void handle_packet_ping(ThreadData* thread_data, uint8_t* buffer, size_t size) {
    PacketPing ping;
    deserialize_ping_packet(buffer, &ping);

    time_t curr_time;
    time(&curr_time);
    printf("[INFO] Recieved a Ping Packet\n");
    printf("    Rawtime:  %zu\n", ping.rawtime);
    printf("    Time Now: %zu\n\n", ping.rawtime);
    
    // Just for now sending new ping packet back 
    server_send_ping_packet(thread_data);
}




void* server_handle_client_send(void* data) {
    ThreadData* thread_data = (ThreadData*) data;

    for (;;) {
        if (thread_data->send_data.new_send_data) {
             
            cthreads_mutex_lock(&thread_data->send_data.send_lock);

            tcs_send(thread_data->client_socket, thread_data->send_data.send_buffer->data, thread_data->send_data.send_buffer->size, TCS_MSG_SENDALL, NULL);
            thread_data->send_data.new_send_data = false;

            printf("[DEBUG] Sending Data!\n");
            
            cthreads_mutex_unlock(&thread_data->send_data.send_lock);
        }
    }
    return NULL;
}

void sever_queue_new_send_data(SendData* send_data, uint8_t* buffer, size_t buffer_size) {
    cthreads_mutex_lock(&send_data->send_lock);
    
    send_data->new_send_data = true;
    d_array_copy(send_data->send_buffer, buffer, buffer_size);

    cthreads_mutex_unlock(&send_data->send_lock);
}

void server_send_ping_packet(ThreadData* thread_data) {
    PacketPing send_packet;
    create_ping_packet(&send_packet);
    size_t packet_size = sizeof(PacketHeader) + sizeof(PacketPing);
    uint8_t send_buffer[packet_size];

    sever_queue_new_send_data(&thread_data->send_data, send_buffer, packet_size);
}

void server_cleanup(Server* server) {
    for (size_t i = 0; i < server->client_count; ++i) {
        if (cthreads_thread_cancel(server->threads->recv_thread) != 0) {
            printf("did not exit thread!\n");
        }
        if (cthreads_thread_cancel(server->threads->send_thread) != 0) {
            printf("did not exit thread!\n");
        }

        tcs_shutdown(server->threads_data[i].client_socket, TCS_SD_BOTH);
        tcs_destroy(&server->threads_data[i].client_socket);

        d_array_free(server->threads_data[i].send_data.send_buffer);
    }

    tcs_shutdown(server->server_socket, TCS_SD_BOTH);
    tcs_destroy(&server->server_socket);

    tcs_lib_free();
}
