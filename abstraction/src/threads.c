#include "threads.h"
#include "packets/packets.h"

void threads_init(ThreadCollection* collection, void (*packet_handle_function) (uint8_t* payload, size_t payload_size), TcsSocket socket) {
    *collection = (ThreadCollection) {0}; // prolly unnecessary
    collection->thread_data = (ThreadData) {
        .packet_handle_function = packet_handle_function,
        .socket = socket,
        .send_data = (SendData) {
            .new_send_data = false,
            .send_buffer = (uint8_t*) malloc(sizeof(uint8_t) * 1),
            .send_buffer_size = 1,
        }
    };
    
    if (collection->thread_data.send_data.send_buffer == NULL) {
        printf("Malloc for new send buffer failed!\n");
        return;
    }
    
    if (cthreads_mutex_init(&collection->thread_data.send_data.send_lock, NULL) != 0) {
        printf("[Error] could not create mutex\n");
        return;
    }
    
    if (cthreads_thread_create(
        &collection->recv_thread,
        NULL,
        threads_run_recv_thread,
        (void*) collection,
        NULL
    ) != 0) {
        printf("[ERROR] An Error Occured While Creating Thread!\n");
        return;
    }

    if (cthreads_thread_create(
        &collection->send_thread,
        NULL,
        threads_run_send_thread,
        (void*) collection,
        NULL
    ) != 0) {
        printf("[ERROR] An Error Occured While Creating Thread!\n");
        return;
    }

}


void* threads_run_recv_thread(void* data) {
    ThreadCollection* collection = (ThreadCollection*) data;

    // just for convenience sake
    ThreadData* thread_data = &collection->thread_data;
    SendData* send_data = &collection->thread_data.send_data;
    
    for(;;) {
        PacketHeader header;
        threads_recv_into_buffer(collection, (uint8_t*) &header, sizeof(PacketHeader));
        
        uint8_t payload[header.length]; // aka the actual packet like PingPacket
        size_t payload_size = header.length;
        threads_recv_into_buffer(collection, payload, payload_size);

        collection->thread_data.packet_handle_function(payload, payload_size);

    }
    return NULL;
}

void* threads_run_send_thread(void* data) {
    ThreadCollection* collection = (ThreadCollection*) data;

    // just for convenience sake
    ThreadData* thread_data = &collection->thread_data;
    SendData* send_data = &collection->thread_data.send_data;

    for (;;) {
        if (send_data->new_send_data) {
            cthreads_mutex_lock(&send_data->send_lock);

            tcs_send(thread_data->socket, send_data->send_buffer, send_data->send_buffer_size, TCS_MSG_SENDALL, NULL);
            send_data->new_send_data = false;

            cthreads_mutex_unlock(&send_data->send_lock);

        }
    }
    return NULL;
}

void threads_recv_into_buffer(ThreadCollection* collection, uint8_t* buffer, size_t size) {
    size_t bytes_recieved = 0;
    
    // so that we can directly recv into a struct like the PacketHeader
    uint8_t tmp_recv_buffer[size];

    while (bytes_recieved < size) {
        tcs_receive(collection->thread_data.socket, tmp_recv_buffer + bytes_recieved, size - bytes_recieved, TCS_NO_FLAGS, &bytes_recieved);
    }

    memcpy(buffer, tmp_recv_buffer, size);

    printf("[DEBUG] recv new Buffer:\n");
    for (size_t i = 0; i < size; ++i) {
            printf("    %zu: %u | 0x%02X\n", i, buffer[i], buffer[i]);
    }
    printf("\n");
}

void threads_queue_new_send_data(ThreadCollection* collection, uint8_t* buffer, size_t buffer_size) {
    SendData* send_data = &collection->thread_data.send_data;
    cthreads_mutex_lock(&send_data->send_lock);

    send_data->new_send_data = true;
    send_data->send_buffer_size = buffer_size;
    send_data->send_buffer = realloc(send_data->send_buffer, send_data->send_buffer_size);

    memcpy(send_data->send_buffer, buffer, buffer_size);

    cthreads_mutex_unlock(&send_data->send_lock);
}

void threads_kill_threads(ThreadCollection* collection) {
    if (cthreads_thread_cancel(collection->recv_thread) != 0) {
        printf("did not exit thread!\n");
    }
    if (cthreads_thread_cancel(collection->send_thread) != 0) {
        printf("did not exit thread!\n");
    }

    tcs_shutdown(collection->thread_data.socket, TCS_SD_BOTH);
    tcs_destroy(&collection->thread_data.socket);
    
    free(collection->thread_data.send_data.send_buffer);
}
