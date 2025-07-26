#include "client.h"
#include "packets/packets.h"
#include "packets/types/data_packet.h"
#include <stdint.h>
#include <string.h>

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
        uint8_t input_buffer[DATA_PACKET_SIZE];
        memset(&input_buffer, 0, DATA_PACKET_SIZE);
        fgets((char*) input_buffer, DATA_PACKET_SIZE, stdin);
        client_send_data_packet(client, &client->threads, (uint8_t*) input_buffer, strlen((char*) input_buffer));
    }
}

void client_handle_packets(void* base_context, PacketHeader header, uint8_t* payload, size_t payload_size) {
    switch (header.type) {
        case PACKET_TYPE_PING:
            client_handle_ping_packet((Client*) base_context, payload, payload_size);
            break;
        case PACKET_TYPE_RAW_DATA:
            client_handle_data_packet((Client*) base_context, payload, payload_size);
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

void client_handle_data_packet(Client* client, uint8_t* payload, size_t payload_size) {
    DataPacket packet;
    memcpy(&packet, payload, payload_size);
    printf("[INFO] Recieved raw data packet\n");
    printf("    ");
    for (size_t i = 0; i < packet.actual_data_size; i++) {
        printf("%c", packet.data[i]);
    }
    printf("\n");
}

void client_send_ping_packet(Client* client, ThreadCollection* collection) {
    PingPacket* payload = ping_packet_create();
    size_t buffer_size;
    Packet* packet = packet_create(PACKET_TYPE_PING, PACKET_HEADER_FLAG_NONE, (void*) payload, sizeof(PingPacket), &buffer_size);

    uint8_t buffer[buffer_size];

    packet_serialize(packet, buffer, buffer_size);

    packet_destory(packet);
    
    threads_queue_new_send_data(collection, buffer, buffer_size);
}

void client_send_data_packet(Client* client, ThreadCollection* collection, uint8_t* data_buffer, size_t data_buffer_size) {
    DataPacket* payload = data_packet_create(data_buffer, data_buffer_size);
    size_t buffer_size;
    Packet* packet = packet_create(PACKET_TYPE_RAW_DATA, PACKET_HEADER_FLAG_SEND_TO_ALL, (void*) payload, sizeof(DataPacket), &buffer_size);

    uint8_t buffer[buffer_size];

    packet_serialize(packet, buffer, buffer_size);
    
    free(packet);
    
    threads_queue_new_send_data(collection, buffer, buffer_size);
}

void client_cleanup(Client* client) {

    threads_kill_threads(&client->threads);

    tcs_shutdown(client->client_socket, TCS_SD_BOTH);
    tcs_destroy(&client->client_socket);

    tcs_lib_free();
}
