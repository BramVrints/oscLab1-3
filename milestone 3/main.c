//
// Created by bram on 29/11/23.
//
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "sbuffer.h"

#define FILENAME "sensor_data"

void *writerThread(void *arg);
void *readerThread(void *arg);

int main() {

    // Shared buffer initialiseren
    sbuffer_t *buffer;
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
    return 0;
}

void *writerThread(void *arg) {
    sbuffer_t *buffer = (sbuffer_t *)arg;

    //bestand openen
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    //sensor data lezen en in de buffer gooien
    sensor_data_t data;
    while (fread(&data, sizeof(sensor_data_t), 1, file) == 1) {
        sbuffer_insert(buffer, &data);
        usleep(10000); //10ms slapen
    }

    //end of stream marker toevoegen
    data.id = 0;
    sbuffer_insert(buffer, &data);

    fclose(file);
    return NULL;
}

void *readerThread(void *arg) {
    sbuffer_t *buffer = (sbuffer_t *)arg;

    //bestand openen
    FILE *csvFile;
    if (buffer == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    sensor_data_t data;
    int status;

    while (1) {
        status = sbuffer_remove(buffer, &data);
        if (status == SBUFFER_SUCCESS) {
            if (data.id == 0) {
                break;
            }

            //data naar het CSV bestand schrijven
            fprintf(csvFile, "%" PRIu16 ",%g,%ld\n", data.id, data.value, (long int)data.ts);
            fflush(csvFile);
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
    return NULL;
}

