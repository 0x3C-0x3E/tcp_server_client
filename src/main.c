#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>

#define TINYCSOCKET_IMPLEMENTATION
#include "libs/tinycsocket.h"

// #define DEBUG_PRINT
#include "server.h"
#include "client.h"

Server server = {0};
Client client = {0};

void cleanup_server(int sig) {
    server.running = false;
    server_cleanup(&server);
    printf("[EXITING] Server exited successfully from SIGINT\n");
    exit(0);
    return;
}


void cleanup_client(int sig) {
    server.running = false;
    server_cleanup(&server);
    printf("[EXITING] Client exited successfully from SIGINT\n");
    exit(0);
    return;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Not enough args!\n");
        printf("    Usage: ./thisfile {server/client}\n");
    }
    
    if (strcmp(argv[1], "server") == 0) {
        signal(SIGINT, cleanup_server);
        goto init_server;

    } else if (strcmp(argv[1], "client") == 0) {
        signal(SIGINT, cleanup_client);
        goto init_client;
    } else {
        printf("Error: Unexpected argument: %s\n", argv[1]);
        return 1;
    }

init_server:
    if (server_init(&server) != 0) {
        return 1;
    }

    server_run(&server);

    server_cleanup(&server);

    goto break_main;

init_client:
    if (client_init(&client) != 0) {
        return 1;
    }

    client_run(&client);

    client_cleanup(&client);

    goto break_main;


break_main:
    return 0;
}
