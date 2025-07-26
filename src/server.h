#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "libs/tinycsocket.h"
#include "packets/packets.h"
#include "libs/cthreads.h"
#include "packets/types/ping_packet.h"
#include "threads/threads.h"

#include "config.h"

#define MAX_CLIENTS 128

typedef struct {
    bool running;
    TcsSocket server_socket;
    
    size_t threads_count; 
    ThreadCollection threads[MAX_CLIENTS];
} Server;

int server_init(Server* server);

void server_run(Server* server);

void server_handle_packets(void* base_context, size_t thread_id, PacketHeader header, uint8_t* payload, size_t payload_size);

void server_handle_ping_packet(Server* server, uint8_t* payload, size_t payload_size);

void server_handle_data_packet(Server* server, uint8_t* payload, size_t payload_size);

void server_send_ping_packet(Server* server, ThreadCollection* collection);

void server_send_to_all(Server* server, PacketHeader header, uint8_t* payload, size_t payload_size, size_t origin_thread);

void server_cleanup(Server* server);
