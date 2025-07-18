#pragma once
#include <time.h>
#include <stdlib.h>

typedef struct {
    time_t ping_time;
} PingPacket;

PingPacket *ping_packet_create();
