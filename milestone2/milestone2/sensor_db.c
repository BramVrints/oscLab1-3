//
// Created by bram on 14/11/23.
//
#include "sensor_db.h"
#include "logger.h"
//#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
//#include <sys/types.h>
#include <wait.h>
#include <inttypes.h>

#define MAX_STR_LEN 255
#define READ_END 0
#define WRITE_END 1

static FILE *csvFile;

FILE * open_db(char * filename, bool append) {
    create_log_process();
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

    return csvFile;
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    if (f == NULL) {
        write_to_log_process("Ongeldige file pointer, sensor kan niet toegevoegd worden :-( ");
        return -1;
    }

    //Zo wordt de data in de file geprint:
    int resultaat = fprintf(f, "%" PRIu16 ",\t%g,\t%" PRIu64 "\n", id, value, (uint64_t)ts);

    //Als er een fout gebeurt, is resultaat < 0, anders stelt resultaat het aantal succesvol geschreven karakters voor
    if (resultaat < 0) {
        write_to_log_process("Er is een fout tijdens het schrijven naar het csv bestand");
        return -1;
    }

    write_to_log_process("Data inserted.");
    return 0;
}

int close_db(FILE * f) {
    if (f == NULL) {
        write_to_log_process("Ongeldige file pointer!");
        return -1;
    }

    fclose(f);
    write_to_log_process("Data file closed.");

    end_log_process();

    return 0;
}
