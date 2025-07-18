#include "data_packet.h"

DataPacket *data_packet_create(uint8_t data[DATA_PACKET_SIZE]) {
    DataPacket *packet = (DataPacket*) malloc(sizeof(DataPacket));

    *packet = (DataPacket) {0};
    memcpy(&packet->data, data, DATA_PACKET_SIZE);

    return packet;
}
