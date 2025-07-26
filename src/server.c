#include "server.h"
#include "packets/packets.h"
#include "packets/types/data_packet.h"
#include "packets/types/ping_packet.h"
#include "threads.h"
#include <sys/types.h>

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
        threads_init(&server->threads[server->threads_count], server_handle_packets, client_socket, (void*) server);
        
        server_send_ping_packet(server, &server->threads[server->threads_count]);

        server->threads_count ++;
    }
}

void server_handle_packets(void* base_context, PacketHeader header, uint8_t* payload, size_t payload_size) {
    switch (header.flag) {
        case PACKET_HEADER_FLAG_NONE:
            break;
        case PACKET_HEADER_FLAG_SEND_TO_ALL:
            server_send_to_all((Server*) base_context, header, payload, payload_size, 0); // TODO: fix this
            break;
        default:
            printf("[ERROR] Unknown Header Flag!\n");
            break;
    }

    switch (header.type) {
        case PACKET_TYPE_PING:
            server_handle_ping_packet((Server*) base_context, payload, payload_size);
            break;
        case PACKET_TYPE_RAW_DATA:
            server_handle_data_packet((Server*) base_context, payload, payload_size);
            break;
        default:
            printf("[ERROR] Unknown Packet Type!\n");
            break;
    }
}

void server_handle_ping_packet(Server* server, uint8_t* payload, size_t payload_size) {
    PingPacket packet;
    memcpy(&packet, payload, payload_size);
    printf("[INFO] Recieved ping packet\n");
    printf("    Ping time: %ld\n", packet.ping_time);
    printf("\n");
}

void server_handle_data_packet(Server* server, uint8_t* payload, size_t payload_size) {
    DataPacket packet;
    memcpy(&packet, payload, payload_size);
    printf("[INFO] Recieved raw data packet\n");
    printf("    ");
    for (size_t i = 0; i < packet.actual_data_size; i++) {
        printf("%c", packet.data[i]);
    }
    printf("\n");
}


void server_send_ping_packet(Server* server, ThreadCollection* collection) {
    PingPacket* payload = ping_packet_create();
    size_t buffer_size;
    Packet* packet = packet_create(PACKET_TYPE_PING, PACKET_HEADER_FLAG_NONE, (void*) payload, sizeof(PingPacket), &buffer_size);

    uint8_t buffer[buffer_size];

    packet_serialize(packet, buffer, buffer_size);

    packet_destory(packet);
    
    threads_queue_new_send_data(collection, buffer, buffer_size);

}

void server_send_to_all(Server* server, PacketHeader header, uint8_t* payload, size_t payload_size, size_t origin_thread) {

    size_t buffer_size;
    Packet* packet = packet_create(header.type, PACKET_HEADER_FLAG_NONE, payload, payload_size, &buffer_size);

    uint8_t buffer[buffer_size];

    packet_serialize(packet, buffer, buffer_size);

    free(packet);

    for (size_t i = 0; i < server->threads_count; ++i) {
        if (i == origin_thread) {
            continue;
        }

        threads_queue_new_send_data(&server->threads[i], buffer, buffer_size);
    }
}

void server_cleanup(Server* server) {
    for (size_t i = 0; i < server->threads_count; ++i) {
        threads_kill_threads(&server->threads[i]);
    }
    tcs_shutdown(server->server_socket, TCS_SD_BOTH);
    tcs_destroy(&server->server_socket);

    tcs_lib_free();
}
