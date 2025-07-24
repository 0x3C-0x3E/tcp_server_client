#include "packets.h"
#include <stdio.h>

Packet *packet_create(uint16_t type, void *payload, size_t payload_size, size_t *serialisation_buffer_size) {
    Packet* packet = (Packet*) malloc(sizeof(Packet));

    *packet = (Packet) {
        .header = (PacketHeader) {
            .type = type,
            .length = payload_size,
        },
        .payload = payload,
    };

    *serialisation_buffer_size = sizeof(PacketHeader) + payload_size;

    return packet;
}

void packet_destory(Packet *packet) {
    if (packet != NULL) {
        free(packet->payload);
        free(packet);
    }
}

void packet_serialize(Packet *packet, uint8_t *buffer, size_t buffer_size) {
    memcpy(buffer, &packet->header, sizeof(PacketHeader));
    memcpy(buffer + sizeof(PacketHeader), packet->payload, packet->header.length);
    printf("[DEBUG] Serialized Buffer:\n");
    for (size_t i = 0; i < buffer_size; ++i) {
            printf("    %zu: %u | 0x%02X\n", i, buffer[i], buffer[i]);
    }
    printf("\n");
}

void packet_deserialize_header(PacketHeader *header, uint8_t* buffer) {
    memcpy(header, buffer, sizeof(PacketHeader));
}

