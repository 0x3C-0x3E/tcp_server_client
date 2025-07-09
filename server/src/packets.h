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

void get_packet_header(uint8_t* buffer, PacketHeader* header_packet);


int create_ping_packet(PacketPing* packet); 

int serialize_ping_packet(uint8_t* buffer, PacketPing* packet); // returning size of buffer

void deserialize_ping_packet(uint8_t* buffer, PacketPing* packet);
