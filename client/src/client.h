#pragma once
#include "libs/tinycsocket.h"
#include "packets.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    bool running;
    TcsSocket client_socket;
} Client;

int client_init(Client* client);

void client_run(Client* client);

void client_recv_header(Client* client, PacketHeader* header);

void client_recv_into_buffer(Client* client, uint8_t* buffer, size_t size);

void client_handle_packet_ping(Client* client, uint8_t* buffer);

void client_cleanup(Client* client);
