#include <stdint.h>
#include <stdio.h>
#define TINYCSOCKET_IMPLEMENTATION
#include "libs/tinycsocket.h"

int main(int argc, char* argv[]) {
    tcs_lib_init();

    TcsSocket client_socket = TCS_NULLSOCKET;
    tcs_create(&client_socket, TCS_TYPE_TCP_IP4);

    tcs_connect(client_socket, "192.168.179.22", 6969);
    
    for (;;) {
        uint8_t recv_buffer[8192] = {0};
        size_t bytes_received = 0;
        tcs_receive(client_socket, recv_buffer, 8192, TCS_NO_FLAGS, &bytes_received);

        printf("%s", recv_buffer);
    }

    tcs_shutdown(client_socket, TCS_SD_BOTH);
    tcs_destroy(&client_socket);

    tcs_lib_free();
}
