//
// Created by bram on 14/11/23.
//
#include "sensor_db.h"
#include "sbuffer.h"
//#include "logger.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <wait.h>
#include <inttypes.h>

#define MAX_STR_LEN 255

//Dit moet in de main.c:
//#define READ_END 0
//#define WRITE_END 1

static FILE *csvFile;
char msg[MAX_STR_LEN];

FILE * open_db(char * filename, bool append) {
    sprintf(msg, "A new data.csv file has been created.");
    write_to_log_process(msg);

    if (append == true) {
        return fopen(filename, "a");
    }
    else {
        return fopen(filename, "w");
    }

//dit moet in main.c komen:
    /*create_log_process();
    char wmsg[MAX_STR_LEN];
    char rmsg[MAX_STR_LEN];
    int logPipe[2];

    if (pipe(logPipe) == -1) {
        perror("Pipe maken voor logger lukt niet");
    }

    pid_t loggerPid = fork();
    if (loggerPid < 0) {
        fprintf(stderr, "Fork mislukt");
        return NULL;
    }

    if (loggerPid == 0) {
        close(logPipe[WRITE_END]); //write einde van de pipe sluiten
        read(logPipe[READ_END], rmsg, MAX_STR_LEN);
        if (strcmp(rmsg, "Bericht") != 0) {
            write_to_log_process("Log file kan niet geopend worden");
        }
        else {
            write_to_log_process("Data file opened.");
        }
        close(logPipe[READ_END]);
        exit(EXIT_SUCCESS);
    }

    else {
        close(logPipe[READ_END]);
        write(logPipe[WRITE_END], "Bericht", strlen("Bericht") + 1);
        if (append) {
            csvFile = fopen(filename, "a");
        }
        else {
            csvFile = fopen(filename, "w");
        }

        if (csvFile == NULL) {
            write(logPipe[WRITE_END], "Mislukt", strlen("Mislukt") +1);
        }
        else {
            write(logPipe[WRITE_END], "Gelukt", strlen("Gelukt") +1);
        }
        close(logPipe[WRITE_END]);
        wait(NULL);
    }

    return csvFile;*/
}

void process_sensor_data_from_sbuffer(FILE * f, sbuffer_t *buffer) {
    sensor_data_t data;
    int result;

    while (1) {
        result = sbuffer_remove(buffer, &data);
        if (result == SBUFFER_SUCCESS) {
            if(insert_sensor(f, data.id, data.value, data.ts) != 0) {
                fprintf(stderr, "Error inserting sensor data into csv file.\n");
            }
        }
        else if (result == SBUFFER_NO_DATA) {
            sleep(1);
        }
        else {
            fprintf(stderr, "Error receiving data from sbuffer.\n");
        }
    }
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    if (f == NULL) {
        write_to_log_process("An error occurred when writing to the csv file.");
        return -1;
    }

    //Zo wordt de data in de file geprint:
    int resultaat = fprintf(f, "%" PRIu16 ",\t%g,\t%" PRIu64 "\n", id, value, (uint64_t)ts);

    //Als er een fout gebeurt, is resultaat < 0, anders stelt resultaat het aantal succesvol geschreven karakters voor
    if (resultaat < 0) {
        write_to_log_process("An error occurred when writing to the csv file.");
        return -1;
    }

    sprintf(msg, "Data insertion from sensor %d succeeded.", id);
    write_to_log_process(msg);
    return 0;
}

int close_db(FILE * f) {
    if (f == NULL) {
        write_to_log_process("Invalid pointer to the csv file");
        return -1;
    }
    else {
        fclose(f);
        write_to_log_process("The data.csv file has been closed.");
        return 0;
    }
}
