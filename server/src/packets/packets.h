#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

typedef enum {
    PACKET_TYPE_PING = 0,
    PACKET_TYPE_RAW_DATA = 0,
    
} PACKET_TYPES;

typedef struct {
    uint16_t type;    
    uint16_t length;
} PacketHeader;

typedef struct {
    PacketHeader header;
    void *payload;
} Packet;

// NOTE: any non generic packet like ping_packet CAN NOT have any pointer to memory outside of the struct itself,
// meaning no heap, or dynamic arrays, which size arent known at compiliation time 

Packet *packet_create(uint16_t type, void *payload, size_t payload_size, size_t *serialisation_buffer_size);

void packet_destory(Packet *packet);

void serialize_packet(Packet *packet, uint16_t *buffer, size_t buffer_size);
