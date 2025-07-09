#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

typedef enum {
    PACKET_PING = 0,
} PACKET_TYPES;

typedef struct {
    uint16_t type;    
    uint16_t length;
} PacketHeader;

typedef struct {
    time_t rawtime; 
} PacketPing;

void create_ping_packet(PacketPing* packet);

// returning the size of the buffer
int serialize_ping_packet(uint8_t* buffer, PacketPing* packet);
