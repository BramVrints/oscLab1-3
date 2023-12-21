//
// Created by bram on 7/12/23.
//
/**
 * \author {Bram Vrints}
 */

#include <stdio.h>
#include "connmgr.h"
#include <stdlib.h>
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

pthread_mutex_t connCounterMutex = PTHREAD_MUTEX_INITIALIZER;
int conn_counter = 0;


int connmgr_main(int MAX_CONN, int PORT, sbuffer_t *buffer) {
    tcpsock_t *server, *client;

    //printf("connmgr.c: Test server is started\n");
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    //printf("connmgr.c: Connectie is geopend\n");
    pthread_t pid[MAX_CONN];
    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    do {
        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) exit(EXIT_FAILURE);

        //Thread maken voor elke client
        ThreadArgs threadArgs;
        threadArgs.client = client;
        threadArgs.buffer = buffer;
        //pthread_t threadId;
        if (pthread_create(&pid[conn_counter], &threadAttr, handle_client, (void *)&threadArgs) != 0) {
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
    //printf("connmgr.c: Writer thread: end of stream marker wordt toegevoegd\n");
    data->id = 0;
    sbuffer_insert(buffer, data);
    //printf("connmgr.c: Test server is shutting down\n");

    return 0;
}

void *handle_client(void *arg) {
    ThreadArgs *threadArgs = (ThreadArgs *)arg;
    tcpsock_t *client = threadArgs->client;
    sbuffer_t *buffer = threadArgs->buffer;
    sensor_data_t data;
    int bytes, result;
    int firstTime = 1;
    char msg[MAX_STR_LEN];

    do {
        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *) &data.id, &bytes);
        if (firstTime == 1) {
            sprintf(msg, "Sensor node %d has opened a new connection.", data.id);
            write_to_log_process(msg);
            firstTime = 0;
        }

        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *) &data.value, &bytes);
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *) &data.ts, &bytes);

        if ((result == TCP_NO_ERROR) && bytes) {
            if (sbuffer_insert(buffer, &data) == SBUFFER_SUCCESS) {
                //printf("connmgr.c: Data in buffer inserted\n");
                /*printf("connmgr.c: sensor id: %d\n", data.id);
                printf("connmgr.c: temperatuur: %lf\n", data.value);
                printf("connmgr.c: timestamp: %ld\n", data.ts);*/
                fflush(stdout);
            }
        }
    } while (result == TCP_NO_ERROR);

    if (result == TCP_CONNECTION_CLOSED) {
        //printf("connmgr.c: Peer has closed connection\n");
        sprintf(msg, "Sensor node %d has closed the connection.", data.id);
        write_to_log_process(msg);
    }
    else {
        //printf("connmgr.c: Error occured on connection to peer\n");
        //printf("result: %d, id: %d\n", result, data.id);
    }

    tcp_close(&client);
    return NULL;
}
