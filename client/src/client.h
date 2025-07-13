#pragma once
#include "libs/cthreads.h"
#include "libs/tinycsocket.h"
#include "libs/d_array.h"
#include "packets.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


typedef struct {
    bool new_send_data; 
    struct cthreads_mutex send_lock;
    D_Array* send_buffer;

} SendData;


typedef struct {
    struct cthreads_thread recv_thread;
    struct cthreads_thread send_thread;

} ThreadCollection;

typedef struct {
    bool running;
    TcsSocket client_socket;

    SendData send_data;
    ThreadCollection threads;
    
} Client;

int client_init(Client* client);

void client_run(Client* client);


void* client_handle_server_recv(void* data);

void client_recv_header(Client* client, PacketHeader* header);

void client_recv_into_buffer(Client* client, uint8_t* buffer, size_t size);


void client_handle_packet(Client* client, PacketHeader* header, uint8_t* buffer, size_t size);

void client_handle_packet_ping(Client* client, uint8_t* buffer, size_t size);


void* client_handle_server_send(void* data);

void client_queue_send_data(Client* client, uint8_t* buffer, size_t buffer_size);

void client_send_ping_packet(Client* client);

void client_cleanup(Client* client);
