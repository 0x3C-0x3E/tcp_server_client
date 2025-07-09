#define TINYCSOCKET_IMPLEMENTATION
#include "libs/tinycsocket.h"
#include <stdint.h>
#include "server.h"


Server server = {0};

int main(int argc, char* argv[]) {
    if (server_init(&server) != 0) {
        return 1;
    }

    server_run(&server);

    server_cleanup(&server);

    return 0;
}
