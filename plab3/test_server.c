/**
 * \author {Bram Vrints}
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include "config.h"
#include "lib/tcpsock.h"

/**
 * Implements a sequential test server (only one connection at the same time)
 */

void *handle_client(void *arg);

pthread_mutex_t printfMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t connCounterMutex = PTHREAD_MUTEX_INITIALIZER;
int conn_counter = 0;


int main(int argc, char *argv[]) {
    tcpsock_t *server, *client;
    
    if(argc < 3) {
    	printf("Please provide the right arguments: first the port, then the max nb of clients");
    	return -1;
    }
    
    int MAX_CONN = atoi(argv[2]);
    int PORT = atoi(argv[1]);

    printf("Test server is started\n");
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("Connectie is geopend");
    do {
        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) exit(EXIT_FAILURE);

        //Thread maken voor elke client
        pthread_t threadId;
        if (pthread_create(&threadId, NULL, handle_client, (void *)client) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        int detachResult = pthread_detach(threadId);
        if (detachResult != 0) {
            perror("pthread_detach");
        }

        //conn_counter incrementen op een thread-safe manier:
        pthread_mutex_lock(&connCounterMutex);
        conn_counter++;
        pthread_mutex_unlock(&connCounterMutex);

    } while (conn_counter < MAX_CONN);

    if (tcp_close(&server) != TCP_NO_ERROR) exit(EXIT_FAILURE);

    printf("Test server is shutting down\n");

    return 0;
}

void *handle_client(void *arg) {
    tcpsock_t *client = (tcpsock_t *)arg;
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
                //kritische sectie bij printf: mutex lock gebruiken
                pthread_mutex_lock(&printfMutex);
                printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
                       (long int) data.ts);
                pthread_mutex_unlock(&printfMutex);
            }
        } while (result == TCP_NO_ERROR);

        if (result == TCP_CONNECTION_CLOSED)
            printf("Peer has closed connection\n");
        else
            printf("Error occured on connection to peer\n");

        tcp_close(&client);

    return NULL;
}

