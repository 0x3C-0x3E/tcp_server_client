#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "libs/tinycsocket.h"
#include "packets/packets.h"
#include "libs/cthreads.h"

typedef struct {
    bool new_send_data; 
    struct cthreads_mutex send_lock;
    uint8_t* send_buffer;
    size_t send_buffer_size;
} SendData;

typedef struct {
    TcsSocket socket;
    void (*packet_handle_function) (void* base_context, PacketHeader header, uint8_t* payload, size_t payload_size);
    void* base_context;

    SendData send_data;
} ThreadData;

typedef struct {
    struct cthreads_thread recv_thread;
    struct cthreads_thread send_thread;

    ThreadData thread_data;

} ThreadCollection;

void threads_init(ThreadCollection* collection, void (*packet_handle_function) (void* base_context, PacketHeader header, uint8_t* payload, size_t payload_size), TcsSocket socket, void* base_context);

void* threads_run_recv_thread(void* data); // ThreadCollection collection

void* threads_run_send_thread(void* data); // ThreadCollection collection

void threads_recv_into_buffer(ThreadCollection* collection, uint8_t* buffer, size_t size);

void threads_queue_new_send_data(ThreadCollection* collection, uint8_t* buffer, size_t buffer_size);

void threads_kill_threads(ThreadCollection* collection);
