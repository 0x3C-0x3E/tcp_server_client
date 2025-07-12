#pragma once
#include "libs/cthreads.h"
#include "libs/tinycsocket.h"
#include "packets.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

struct ClientSendData {
    struct cthreads_mutex send_data_lock;

    bool new_send_data;

    size_t buffer_size;
    uint8_t* buffer;
};

typedef struct {
    bool running;
    TcsSocket client_socket;
    
    struct ClientSendData send_data;
    struct cthreads_thread send_thread;
    
    struct cthreads_thread recv_thread;
} Client;

int client_init(Client* client);

void client_run(Client* client);

void* client_run_send_thread(void* data);

void client_queue_send_data(Client* client, uint8_t* buffer, size_t buffer_size);

void client_recv_header(Client* client, PacketHeader* header);

void client_recv_into_buffer(Client* client, uint8_t* buffer, size_t size);

void client_handle_packet_ping(Client* client, uint8_t* buffer);

void client_cleanup(Client* client);
