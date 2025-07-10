#include "client.h"
#include <stdio.h>
#include <string.h>

#define PORT 6969


int client_init(Client* client) {

    if (tcs_lib_init() != 0) {
        printf("Could Not Init TCS LIB\n");
        return 1;
    }

    *client = (Client) {
        .running = true,
        .client_socket = TCS_NULLSOCKET,
    };
    
    if (tcs_create(&client->client_socket, TCS_TYPE_TCP_IP4) != 0) {
        printf("Could not init client socket!\n");
        return 1;
    }


    if (tcs_connect(client->client_socket, "192.168.179.22", PORT)){
        printf("Could not Connect to Socket at Port: %d\n", PORT);
        return 1;
    }
    
    return 0;
}

void client_run(Client* client) {
    for (;;) {
        PacketHeader header;
        client_recv_header(client, &header);

        uint8_t recv_buffer[header.length];
        client_recv_into_buffer(client, recv_buffer, header.length);

        switch (header.type) {
            case PACKET_PING:
                client_handle_packet_ping(client, recv_buffer);
                break;
        }
    }
}


void client_recv_header(Client* client, PacketHeader* header) {
    uint8_t recv_buffer[sizeof(PacketHeader)];
    client_recv_into_buffer(client, recv_buffer, sizeof(recv_buffer));;

    get_packet_header(recv_buffer, header);

    printf("[INFO] Deserialized Packet Header:\n");
    printf("    Type: %d\n", header->type);
    printf("    Length: %d\n", header->length);
}

void client_recv_into_buffer(Client* client, uint8_t* buffer, size_t size) {
    tcs_receive(client->client_socket, buffer, size, TCS_NO_FLAGS, NULL);
}

void client_handle_packet_ping(Client* client, uint8_t* buffer) {
    PacketPing ping;
    deserialize_ping_packet(buffer, &ping);
    printf("[INFO] Recieved a Ping Packet\n");
    printf("    Rawtime: %zu\n", ping.rawtime);
}

void client_cleanup(Client* client) {
    tcs_shutdown(client->client_socket, TCS_SD_BOTH);
    tcs_destroy(&client->client_socket);

    tcs_lib_free();
}
