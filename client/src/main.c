#include <signal.h>
#include <stdint.h>
#define TINYCSOCKET_IMPLEMENTATION
#include "libs/tinycsocket.h"
#include "client.h"

Client client = {0};

void cleanup(int sig) {
    client_cleanup(&client);
    printf("[EXITING] Client exited successfully from SIGINT\n");
    exit(69);
}

int main(int argc, char* argv[]) {
    
    signal(SIGINT, cleanup);

    if (client_init(&client) != 0) {
        return 1;
    }

    client_run(&client);
    
    client_cleanup(&client);
}
