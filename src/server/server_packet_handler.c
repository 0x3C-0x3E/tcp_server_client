#include "server_packet_handler.h"


void server_handle_packets(void* base_context, size_t thread_id, PacketHeader header, uint8_t* payload, size_t payload_size) {
    switch (header.flag) {
        case PACKET_HEADER_FLAG_NONE:
            break;
        case PACKET_HEADER_FLAG_SEND_TO_ALL:
            server_send_to_all((Server*) base_context, header, payload, payload_size, thread_id);
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

