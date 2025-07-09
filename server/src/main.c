#include <stdlib.h>
#define TINYCSOCKET_IMPLEMENTATION
#include "libs/tinycsocket.h"
#include <stdint.h>
#include "server.h"
#include <signal.h>


Server server = {0};

void cleanup(int sig) {
    server.running = false;
    server_cleanup(&server);
    printf("[EXITING] Server exited successfully from SIGINT\n");
    exit(69); // maybe change that ...
}

int main(int argc, char* argv[]) {
    signal(SIGINT, cleanup);
    if (server_init(&server) != 0) {
        return 1;
    }

    server_run(&server);

    server_cleanup(&server);

    return 0;
}
