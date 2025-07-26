#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DATA_PACKET_SIZE 64 

typedef struct {
    size_t actual_data_size;
    uint8_t data[DATA_PACKET_SIZE];
} DataPacket;

// uint8_t* data is a buffer that HAS to be DATA_PACKET_SIZE
DataPacket* data_packet_create(uint8_t* data, size_t actual_data_size);
