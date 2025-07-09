#include "server.h"

int server_init(Server* server) {
    if (tcs_lib_init() != TCS_SUCCESS) {
        return 1;
    }

    printf("[INFO] Starting Server\n");

    server->running = true;

    server->server_socket = TCS_NULLSOCKET;
    if (tcs_create(&server->server_socket, TCS_TYPE_TCP_IP4) != TCS_SUCCESS) {
        return 1;
    }

    if (tcs_listen_to(server->server_socket, PORT) != TCS_SUCCESS) {
        return 1;
    }
    
    printf("[INFO] Listening on Port %d ...\n", PORT);

    return 0;
}

void server_run(Server* server) {
    while (server->running) {
        TcsSocket client_socket = TCS_NULLSOCKET;
        tcs_accept(server->server_socket, &client_socket, NULL);

        ThreadData thread_data = {
            .server = server,
            .client_socket = client_socket,
        };

        server->threads[server->num_clients] = (struct cthreads_thread) {0};
        struct cthreads_thread* thread = &server->threads[server->num_clients];

        server->threads_data[server->num_clients] = thread_data;

        if (cthreads_thread_create(thread, NULL, server_handle_client, (void*) &server->threads_data[server->num_clients], NULL) != 0) {
            printf("[ERROR] An Error Occured While Creating Thread!\n");
        }

        server->num_clients ++;
    }

    for (size_t i = 0; i < server->num_clients; ++i) {
        cthreads_thread_join(server->threads[i], NULL);
    }
}

void* server_handle_client(void* data) {
    ThreadData thread_data = *(ThreadData*) data;
    Server* server = thread_data.server;
    TcsSocket client_socket = thread_data.client_socket;

    printf("[INFO] New Client Accepted\n");

    uint8_t b[] = "Hello from server!\n";
    tcs_send(client_socket, b, sizeof(b), TCS_MSG_SENDALL, NULL);

    for (;;) {

    }

    tcs_shutdown(client_socket, TCS_SD_BOTH);
    printf("[INFO] Closed Client Connection\n");
    tcs_destroy(&client_socket);

    return NULL;
}

void send_message(TcsSocket client_socket, uint8_t* buffer, size_t buffer_size) {
    tcs_send(client_socket, buffer, buffer_size, TCS_MSG_SENDALL, NULL);
}

void server_cleanup(Server* server) {
    tcs_shutdown(server->server_socket, TCS_SD_BOTH);
    tcs_destroy(&server->server_socket);

    tcs_lib_free();
}
