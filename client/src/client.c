#include "client.h"
#include "libs/cthreads.h"
#include "libs/d_array.h"
#include "packets.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PORT 6969

int client_init(Client* client) {
    if (tcs_lib_init() != 0) {
        printf("Could Not Init TCS LIB\n");
        return 1;
    }

    *client = (Client) {
        .running = true,
        .client_socket = TCS_NULLSOCKET,
        .send_data = (SendData) {
            .send_buffer = d_array_new(sizeof(uint8_t)),
            .new_send_data = false,
        },
    };

    if (cthreads_mutex_init(&client->send_data.send_lock, NULL) != 0) {
        printf("Could not create mutext!\n");
        return 1;
    }
    
    if (tcs_create(&client->client_socket, TCS_TYPE_TCP_IP4) != 0) {
        printf("Could not init client socket!\n");
        return 1;
    }

    if (tcs_connect(client->client_socket, "192.168.179.22", PORT)) {
        printf("Could not Connect to Socket at Port: %d\n", PORT);
        return 1;
    }


    if (cthreads_thread_create(&client->threads.recv_thread, NULL, client_handle_server_recv, (void*) client, NULL) != 0) {
        printf("Could not create cthread!\n");
        return 1;
    }

    if (cthreads_thread_create(&client->threads.send_thread, NULL, client_handle_server_send, (void*) client, NULL) != 0) {
        printf("Could not create cthread!\n");
        return 1;
    }


    
    return 0;
}

void client_run(Client* client) {
    for (;;) {

    }
    cthreads_thread_join(client->threads.recv_thread, NULL);
    cthreads_thread_join(client->threads.send_thread, NULL);
}

void* client_handle_server_recv(void* data) {
    Client* client = (Client*) data;
        
    for (;;) {
        PacketHeader header;
        client_recv_header(client, &header);

        uint8_t recv_buffer[header.length];
        client_recv_into_buffer(client, recv_buffer, header.length);

        client_handle_packet(client, &header, recv_buffer, header.length);
    }

    return NULL;
}

void client_recv_header(Client* client, PacketHeader* header) {
    uint8_t recv_buffer[sizeof(PacketHeader)];
    client_recv_into_buffer(client, recv_buffer, sizeof(recv_buffer));;

    get_packet_header(recv_buffer, header);

    printf("[INFO] Deserialized Packet Header:\n");
    printf("    Type: %d\n", header->type);
    printf("    Length: %d\n", header->length);
    
}

void client_recv_into_buffer(Client* client, uint8_t* buffer, size_t size) {
    tcs_receive(client->client_socket, buffer, size, TCS_NO_FLAGS, NULL);
}

void client_handle_packet(Client* client, PacketHeader* header, uint8_t* buffer, size_t size) {
    switch (header->type) {
        case PACKET_PING:
            client_handle_packet_ping(client, buffer, size);
            break;
    }
}

void client_handle_packet_ping(Client* client, uint8_t* buffer, size_t size) {
    PacketPing ping;
    deserialize_ping_packet(buffer, &ping);

    time_t curr_time;
    time(&curr_time);
    printf("[INFO] Recieved a Ping Packet\n");
    printf("    Rawtime:  %zu\n", ping.rawtime);
    printf("    Time Now: %zu\n\n", ping.rawtime);


    // Just for now just send a ping packet back
    PacketPing packet;
    create_ping_packet(&packet);
    int size_packet = sizeof(PacketHeader) + sizeof(PacketPing);
    uint8_t send_buffer[size_packet];
    serialize_ping_packet(send_buffer, &packet);

    client_queue_send_data(client, send_buffer, sizeof(send_buffer));
}


void* client_handle_server_send(void* data) {
    Client* client = (Client*) data;

    for (;;) {
        if (client->send_data.new_send_data) {
            cthreads_mutex_lock(&client->send_data.send_lock);

            tcs_send(client->client_socket, client->send_data.send_buffer->data, client->send_data.send_buffer->size, TCS_MSG_SENDALL, NULL);
            client->send_data.new_send_data = false;

            cthreads_mutex_unlock(&client->send_data.send_lock);
        }
    }
}

void client_queue_send_data(Client* client, uint8_t* buffer, size_t buffer_size) {
    cthreads_mutex_lock(&client->send_data.send_lock);
    
    client->send_data.new_send_data = true;
    d_array_copy(client->send_data.send_buffer, buffer, buffer_size);

    cthreads_mutex_unlock(&client->send_data.send_lock);

}

void client_send_ping_packet(Client* client) {
    PacketPing packet;
    create_ping_packet(&packet);

    size_t packet_size = sizeof(PacketHeader) + sizeof(PacketPing);
    uint8_t send_buffer[packet_size];
    client_queue_send_data(client, send_buffer, packet_size);
}

void client_cleanup(Client* client) {
    tcs_shutdown(client->client_socket, TCS_SD_BOTH);
    tcs_destroy(&client->client_socket);

    cthreads_mutex_destroy(&client->send_data.send_lock);
    d_array_free(client->send_data.send_buffer);

    tcs_lib_free();
}
