#include "data_packet.h"

DataPacket* data_packet_create(uint8_t* data, size_t actual_data_size) {
    DataPacket *packet = (DataPacket*) malloc(sizeof(DataPacket));

    *packet = (DataPacket) {0};
    memcpy(&packet->data, data, DATA_PACKET_SIZE);
    packet->actual_data_size = actual_data_size;

    return packet;
}
