#pragma once
#include <stdint.h>
#include <stdlib.h>
#define DATA_PACKET_SIZE 255

typedef struct {
    uint8_t data[DATA_PACKET_SIZE];
} DataPacket;

DataPacket *data_packet_create(uint8_t *data);
