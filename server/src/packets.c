#include "packets.h"
#include <string.h>

void get_packet_header(uint8_t* buffer, PacketHeader* header_packet) {
    memcpy(header_packet, buffer, sizeof(PacketHeader));
}

void create_ping_packet(PacketPing* packet) {
    *packet = (PacketPing) {0};
    time(&packet->rawtime);
}

int serialize_ping_packet(uint8_t* buffer, PacketPing* packet) {
    PacketHeader header = {
        .type = PACKET_PING,
        .length = sizeof(PacketPing),
    };
    
    memcpy(buffer, &header, sizeof(PacketHeader));
    memcpy(buffer + sizeof(PacketHeader), packet, sizeof(PacketPing));

    return sizeof(PacketHeader) + sizeof(PacketPing);
}

void deserialize_ping_packet(uint8_t* buffer, PacketPing* packet) {
    uint8_t* packet_addr = buffer + sizeof(PacketHeader);
    memcpy(packet, packet_addr, sizeof(PacketPing));
}
