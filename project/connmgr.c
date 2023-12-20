//
// Created by bram on 7/12/23.
//
/**
 * \author {Bram Vrints}
 */

#include <stdio.h>
#include "connmgr.h"
#include <stdlib.h>
//#include <inttypes.h>
#include <pthread.h>
#include "config.h"
#include "sbuffer.h"
#include "lib/tcpsock.h"

/**
 * Implements a parallel test server (multiple connections at the same time)
 */

typedef struct {
    tcpsock_t *client;
    sbuffer_t *buffer;
} ThreadArgs;

//void *handle_client(void *arg);

pthread_mutex_t connCounterMutex = PTHREAD_MUTEX_INITIALIZER;
int conn_counter = 0;

int connmgr_main(int MAX_CONN, int PORT, sbuffer_t *buffer) {
    tcpsock_t *server, *client;


    /*// Shared buffer initialization
    sbuffer_t *buffer;
    if (sbuffer_init(&buffer) != SBUFFER_SUCCESS) {
        fprintf(stderr, "Buffer initialization failed\n");
        exit(EXIT_FAILURE);
    }*/


    printf("Test server is started\n");
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("Connectie is geopend");
    pthread_t pid[MAX_CONN];
    do {
        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) exit(EXIT_FAILURE);

        //Thread maken voor elke client
        ThreadArgs threadArgs;
        threadArgs.client = client;
        threadArgs.buffer = buffer;
        //pthread_t threadId;
        if (pthread_create(&pid[conn_counter], NULL, handle_client, (void *)&threadArgs) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        //conn_counter incrementen op een thread-safe manier:
        pthread_mutex_lock(&connCounterMutex);
        conn_counter++;
        pthread_mutex_unlock(&connCounterMutex);

    } while (conn_counter < MAX_CONN);


    if (tcp_close(&server) != TCP_NO_ERROR) exit(EXIT_FAILURE);

    for (int i = 0; i<MAX_CONN; i++) {
        pthread_join(pid[i], NULL);
    }

    //end of stream marker toevoegen
    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    printf("Writer thread: end of stream marker wordt toegevoegd\n");
    data->id = 0;
    sbuffer_insert(buffer, data);
    printf("Test server is shutting down\n");

    return 0;
}

void *handle_client(void *arg) {
    ThreadArgs *threadArgs = (ThreadArgs *)arg;
    tcpsock_t *client = threadArgs->client;
    sbuffer_t *buffer = threadArgs->buffer;
    sensor_data_t data;
    int bytes, result;

    do {
        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *) &data.id, &bytes);
        printf("sensor id ontvangen\n");
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *) &data.value, &bytes);
        printf("temperatuur ontvangen\n");
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *) &data.ts, &bytes);
        printf("timestamp ontvangen\n");

        if ((result == TCP_NO_ERROR) && bytes) {
            //Instantie van sensor data kopiëren om thread safety te waarborgen
            //Anders kan het zijn dat de client de data aanpastt vooraleer het gekopiëerd is in de buffer
            //Want de mutex lock zit IN de sbuffer_insert
            sensor_data_t dataToCopy;
            dataToCopy.id = data.id;
            dataToCopy.value = data.value;
            dataToCopy.ts = data.ts;
            if (sbuffer_insert(buffer, &dataToCopy) == SBUFFER_SUCCESS) {
                printf("Data in buffer inserted\n");
            }
        }
    } while (result == TCP_NO_ERROR);

    if (result == TCP_CONNECTION_CLOSED)
        printf("Peer has closed connection\n");
    else
        printf("Error occured on connection to peer\n");

    tcp_close(&client);

    return NULL;
}

