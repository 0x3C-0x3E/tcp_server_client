#pragma once
#include "libs/cthreads.h"
#include "libs/tinycsocket.h"
#include "packets/packets.h"
#include "threads/threads.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "config.h"

typedef struct {
    bool running;
    TcsSocket client_socket;

    ThreadCollection threads;
    
} Client;


int client_init(Client* client);

void client_run(Client* client);

void client_handle_packets(void* base_context, size_t thread_id, PacketHeader header, uint8_t* payload, size_t payload_size);

void client_handle_ping_packet(Client* client, uint8_t* payload, size_t payload_size);

void client_handle_data_packet(Client* client, uint8_t* payload, size_t payload_size);

void client_send_ping_packet(Client* client, ThreadCollection* collection);

void client_send_data_packet(Client* client, ThreadCollection* collection, uint8_t* data_buffer, size_t data_buffer_size);

void client_cleanup(Client* client);
