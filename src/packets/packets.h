#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "types/data_packet.h"
#include "types/ping_packet.h"

typedef enum {
    PACKET_TYPE_PING = 0,
    PACKET_TYPE_RAW_DATA = 1,
    
} PACKET_TYPES;

typedef enum {
    PACKET_HEADER_FLAG_NONE = 0, 
    PACKET_HEADER_FLAG_SEND_TO_ALL = 1, 
} PACKET_HEADER_FLAGS;

typedef struct {
    uint16_t type;
    uint16_t flag;
    uint16_t length;

} PacketHeader;

typedef struct {
    PacketHeader header;
    void *payload;
} Packet;

// NOTE: any non generic packet like ping_packet CAN NOT have any pointer to memory outside of the struct itself,
// meaning no heap, or dynamic arrays, which size arent known at compiliation time 

Packet *packet_create(uint16_t type, uint16_t flag, void *payload, size_t payload_size, size_t *serialisation_buffer_size);

void packet_destory(Packet *packet);

void packet_serialize(Packet *packet, uint8_t *buffer, size_t buffer_size);

void packet_deserialize_header(PacketHeader *header, uint8_t* buffer);
