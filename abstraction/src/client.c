#include "client.h"

int client_init(Client* client) {
    if (tcs_lib_init() != 0) {
        printf("Could Not Init TCS LIB\n");
        return 1;
    }

    *client = (Client) {
        .running = true,
        .client_socket = TCS_NULLSOCKET,
    };

    if (tcs_create(&client->client_socket, TCS_TYPE_TCP_IP4) != 0) {
        printf("Could not init client socket!\n");
        return 1;
    }

    if (tcs_connect(client->client_socket, "192.168.179.22", PORT)) {
        printf("Could not Connect to Socket at Port: %d\n", PORT);
        return 1;
    }

    threads_init(&client->threads, client_handle_packets, client->client_socket, (void*) client);


    printf("[INFO] Conn with Server established\n");
    return 0;
}

void client_run(Client* client) {
    for (;;) {
    
    }
}

void client_handle_packets(void* base_context, PacketHeader header, uint8_t* payload, size_t payload_size) {
    switch (header.type) {
        case PACKET_TYPE_PING:
            client_handle_ping_packet((Client*) base_context, payload, payload_size);
            break;
        default:
            printf("[ERROR] Unknown Packet Type!\n");
            break;
    }

}

void client_handle_ping_packet(Client* client, uint8_t* payload, size_t payload_size) {
    PingPacket packet;
    memcpy(&packet, payload, payload_size);
    printf("[INFO] Recieved ping packet\n");
    printf("    Ping time: %ld\n", packet.ping_time);
    printf("\n");

    client_send_ping_packet(client, &client->threads);
}

void client_send_ping_packet(Client* client, ThreadCollection* collection) {
    PingPacket* payload = ping_packet_create();
    size_t buffer_size;
    Packet* packet = packet_create(PACKET_TYPE_PING, (void*) payload, sizeof(PingPacket), &buffer_size);

    uint8_t buffer[buffer_size];

    packet_serialize(packet, buffer, buffer_size);

    packet_destory(packet);
    
    threads_queue_new_send_data(collection, buffer, buffer_size);
}

void client_cleanup(Client* client) {

    threads_kill_threads(&client->threads);

    tcs_shutdown(client->client_socket, TCS_SD_BOTH);
    tcs_destroy(&client->client_socket);

    tcs_lib_free();
}
