#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "libs/tinycsocket.h"
#include "packets/packets.h"
#include "libs/cthreads.h"

#define PORT 6969
#define MAX_CLIENTS 128

typedef struct Server Server;

typedef struct {
    bool new_send_data; 
    struct cthreads_mutex send_lock;
    uint8_t* send_buffer;
    size_t send_buffer_size;

} SendData;

typedef struct {
    struct cthreads_thread recv_thread;
    struct cthreads_thread send_thread;

} ThreadCollection;

typedef struct {
    Server* server;
    TcsSocket client_socket;

    // used for the recv thread to find the send thread
    ThreadCollection* other_threads;
    SendData send_data;

} ThreadData;

typedef struct Server {
    bool running;
    TcsSocket server_socket;
    
    size_t client_count; 
    ThreadCollection threads[MAX_CLIENTS];
    ThreadData threads_data[MAX_CLIENTS];
} Server;


int server_init(Server* server);

void server_run(Server* server);


void* server_handle_client_recv(void* data);

void thread_recv_header(ThreadData* thread_data, PacketHeader* header);

void thread_recv_into_buffer(ThreadData* thread_data, uint8_t* buffer, size_t size);


void thread_handle_packet(ThreadData* thread_data, PacketHeader* header, uint8_t* buffer, size_t size);

void handle_packet_ping(ThreadData* thread_data, uint8_t* buffer, size_t size);



void* server_handle_client_send(void* data);

void sever_queue_new_send_data(SendData* send_data, uint8_t* buffer, size_t buffer_size);

void server_send_ping_packet(ThreadData* thread_data);

void server_cleanup(Server* server);
