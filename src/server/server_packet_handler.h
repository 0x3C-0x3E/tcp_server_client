#pragma once
#include "server.h"

void server_handle_packets(void* base_context, size_t thread_id, PacketHeader header, uint8_t* payload, size_t payload_size);

void server_handle_ping_packet(Server* server, uint8_t* payload, size_t payload_size);

void server_handle_data_packet(Server* server, uint8_t* payload, size_t payload_size);
