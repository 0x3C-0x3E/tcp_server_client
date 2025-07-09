#include "packets.h"
#include <string.h>

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
    memcpy(buffer + sizeof(PacketHeader), &header, sizeof(PacketHeader));

    return sizeof(PacketHeader) + sizeof(PacketPing);
}

