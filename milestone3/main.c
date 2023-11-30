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

void *writerThread();
void *readerThread();

//mutex initialiseren om veilig naar de csv file te kunnen schrijven
pthread_mutex_t csvFileMutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    // Shared buffer initialiseren
    if (sbuffer_init(&buffer) != SBUFFER_SUCCESS) {
        fprintf(stderr, "Buffer initialiseren mislukt :( \n");
        exit(EXIT_FAILURE);
    }

    //threads maken
    pthread_t writer, reader1, reader2;
    if (pthread_create(&writer, NULL, writerThread, (void *)buffer) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&reader1, NULL, readerThread, (void *)buffer) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&reader2, NULL, readerThread, (void *)buffer) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    //wachten tot de threads klaar zijn
    pthread_join(writer, NULL);
    pthread_join(reader1, NULL);
    pthread_join(reader2, NULL);

    //opruimen
    sbuffer_free(&buffer);
    pthread_mutex_destroy(&csvFileMutex);

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
        sbuffer_insert(buffer, data);
        printf("Writer thread heeft sensor data gelezen - ID: %" PRIu16 "\n", data->id);
        usleep(10000); //10ms slapen

    }

    //end of stream marker toevoegen
    printf("Writer thread: end of stream marker wordt toegevoegd\n");
    data->id = 0;
    sbuffer_insert(buffer, data);
    //usleep(50000);

    fclose(file);
    free(data);
    pthread_exit(NULL);
}

void *readerThread() {

    sensor_data_t *data = (sensor_data_t *)malloc(sizeof(sensor_data_t));
    int status;

    //bestand openen
    FILE *csvFile = fopen("sensor_data_out.csv", "w");
    if (csvFile == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        status = sbuffer_remove(buffer, data);
        if (status == SBUFFER_SUCCESS) {
            if (data->id == 0) {
                printf("reader thread: end of stream marker gedetecteerd");
                break;
            }

            //data naar het CSV bestand schrijven: kritische sectie
            pthread_mutex_lock(&csvFileMutex);
            fprintf(csvFile, "%" PRIu16 ",%g,%ld\n", data->id, data->value, data->ts);
            printf("Reader thread heeft data geschreven naar csv bestand - ID: %" PRIu16 "\n", data->id);
            fflush(csvFile);
            pthread_mutex_unlock(&csvFileMutex);
            usleep(25000); //25ms slapen
        }
        else if (status == SBUFFER_NO_DATA) {
            usleep(10000); //als er geen data beschikbaar is gaan we 10ms slapen
        }
        else {
            perror("sbuffer_remove");
            exit(EXIT_FAILURE);
        }
    }
    fclose(csvFile);
    free(data);
    pthread_exit(NULL);
}

