#include "server.h"
#include "packets/packets.h"
#include "packets/types/ping_packet.h"
#include "threads.h"

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
        threads_init(&server->threads[server->threads_count], server_handle_packets, server->server_socket, (void*) server);

        server->threads_count ++;
    }
}

void server_handle_packets(void* base_context, PacketHeader header, uint8_t* payload, size_t payload_size) {
    switch (header.type) {
        case PACKET_TYPE_PING:
            server_handle_ping_packet((Server*) base_context, payload, payload_size);
    }
}

void server_handle_ping_packet(Server* server, uint8_t* payload, size_t payload_size) {
    PingPacket packet;
    memcpy(&packet, payload, payload_size);
    printf("[INFO] Recieved ping packet\n");
    printf("    Ping time: %ld\n", packet.ping_time);
    printf("\n");
}

void server_cleanup(Server* server) {
    for (size_t i = 0; i < server->threads_count; ++i) {
        threads_kill_threads(&server->threads[i]);
    }
    tcs_shutdown(server->server_socket, TCS_SD_BOTH);
    tcs_destroy(&server->server_socket);

    tcs_lib_free();
}
