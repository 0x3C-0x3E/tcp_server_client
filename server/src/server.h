#pragma once
#include "libs/tinycsocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "libs/cthreads.h"

#define PORT 6969
#define MAX_CLIENTS 128

typedef struct Server Server;

typedef struct {
    Server* server;
    TcsSocket client_socket;
} ThreadData;

typedef struct Server {
    bool running;
    TcsSocket server_socket;
    
    size_t num_clients;
    struct cthreads_thread threads[MAX_CLIENTS];
    ThreadData threads_data[MAX_CLIENTS];
} Server;

int server_init(Server* server);

void server_run(Server* server);

void* server_handle_client(void* data);

void send_message(TcsSocket client_socket, uint8_t* buffer, size_t buffer_size);

void server_cleanup(Server* server);
