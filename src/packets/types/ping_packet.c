#include "ping_packet.h"

PingPacket *ping_packet_create() {
    time_t timer;
    time(&timer);

    PingPacket *packet = (PingPacket*) malloc(sizeof(PingPacket));
    *packet = (PingPacket) {
        .ping_time = timer,
    };

    return packet;
}
