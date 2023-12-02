//
// Created by bram on 29/11/23.
//
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <inttypes.h>
#include "sbuffer.h"

#define FILENAME "sensor_data"

sbuffer_t *buffer;
FILE *csvFile;

void *writerThread();
void *readerThread();

//mutex initialiseren om veilig naar de csv file te kunnen schrijven
pthread_mutex_t csvFileMutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    //csv bestand openen
    csvFile = fopen("sensor_data_out.csv", "a");
    if (csvFile == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Shared buffer initialiseren
    if (sbuffer_init(&buffer) != SBUFFER_SUCCESS) {
        fprintf(stderr, "Buffer initialiseren mislukt :( \n");
        exit(EXIT_FAILURE);
    }

    //threads maken
    pthread_t writer, reader1, reader2;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (pthread_create(&writer, &attr, writerThread, (void *)buffer) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&reader1, &attr, readerThread, (void *)buffer) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&reader2, &attr, readerThread, (void *)buffer) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    //wachten tot de threads klaar zijn
    pthread_join(writer, NULL);
    pthread_join(reader1, NULL);
    pthread_join(reader2, NULL);

    //opruimen
    sbuffer_free(&buffer);
    //pthread_mutex_destroy(&csvFileMutex);
    //pthread_mutex_destroy(&bufferMutex);

    return 0;
}

void *writerThread() {

    //bestand openen: rb wilt zeggen read binary
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    //sensor data lezen en in de buffer gooien
    sensor_data_t *data = (sensor_data_t *)malloc(sizeof(sensor_data_t));
    while (fread(&data->id, sizeof(sensor_id_t), 1, file) == 1) {
        fread(&data->value, sizeof(sensor_value_t), 1, file);
        fread(&data->ts, sizeof(sensor_ts_t), 1, file);
        //pthread_mutex_lock(&bufferMutex);
        sbuffer_insert(buffer, data);
        //pthread_mutex_unlock(&bufferMutex);
        printf("Writer thread heeft sensor data gelezen - ID: %" PRIu16 "\n", data->id);
        usleep(10000); //10ms slapen

    }

    //end of stream marker toevoegen
    printf("Writer thread: end of stream marker wordt toegevoegd\n");
    data->id = 0;
    //pthread_mutex_lock(&bufferMutex);
    sbuffer_insert(buffer, data);
    //pthread_mutex_unlock(&bufferMutex);
    //usleep(50000);

    fclose(file);
    free(data);
    pthread_exit(NULL);
    //return NULL;
}

void *readerThread() {

    sensor_data_t *data = (sensor_data_t *)malloc(sizeof(sensor_data_t));
    int status;

    while (1) {
        status = sbuffer_remove(buffer, data);

        if (status == SBUFFER_SUCCESS) {
            if (data->id == 0) {
                printf("reader thread: end of stream marker gedetecteerd\n");

                break;
            }

            //data naar het CSV bestand schrijven: kritische sectie
            pthread_mutex_lock(&csvFileMutex);
            fprintf(csvFile, "%d, %f, %ld\n", data->id, data->value, data->ts);
            printf("Reader thread heeft data geschreven naar csv bestand - ID: %" PRIu16 "\n", data->id);
            fflush(csvFile);
            pthread_mutex_unlock(&csvFileMutex);
            usleep(25000); //25ms slapen
        }
        //else if (status == SBUFFER_NO_DATA) {
            //usleep(10000); //als er geen data beschikbaar is gaan we 10ms slapen
        //}
        else {
            perror("sbuffer_remove");
            exit(EXIT_FAILURE);
        }

    }
    //fclose(csvFile);
    free(data);
    pthread_exit(NULL);
    //return NULL;
}

