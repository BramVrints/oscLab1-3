//
// Created by bram on 14/11/23.
//
#include "sensor_db.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <wait.h>

static FILE *csvFile = NULL;
static int logPipe[2];

FILE * open_db(char * filename, bool append) {
    csvFile = fopen(filename, append ? "a" : "w");
    if (csvFile == NULL) {
        perror("Bestand kan niet geopend worden");
        exit(EXIT_FAILURE);
    }

    if (pipe(logPipe) == -1) {
        perror("Pipe maken voor logger lukt niet");
    }

    pid_t loggerPid = fork();
    if (loggerPid < 0) {
        perror("Fork mislukt");
    }
    if (loggerPid == 0) {
        close(logPipe[1]); //write einde van de pipe sluiten
        char logMsg[256];

        while (read(logPipe[0], logMsg, sizeof(logMsg)) > 0) {
            //De log message is ontvangen, dus we gaan schrijven naar gateway.log
            FILE *logFile = fopen("gateway.log", "a");
            if (logFile == NULL) {
                perror("Log file kan niet geopend worden");
            }
            fprintf(logFile, "%s\n", logMsg);
            fclose(logFile);
        }
        exit(EXIT_SUCCESS);
    }

    close(logPipe[0]); //read einde van de pipe sluiten
    char logMsg[256];
    sprintf(logMsg, "Er is een nieuw .csv bestand gemaakt of er is een bestaand bestand bijgewerkt."
                    " Logger PID: %d)", loggerPid);
    write(logPipe[1], logMsg, strlen(logMsg)+1);
    return csvFile;
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    //Zo wordt de data in de file geprint:
    int resultaat = fprintf(f, "%" PRIu16 ",\t%g,\t%" PRIu64 "\n", id, value, (uint64_t)ts);
    //Als er een fout gebeurt is resultaat < 0, anders stelt deze het aantal succesvol geschreven karakters voor
    if (resultaat < 0) {
        perror("Er kan niet naar het bestand geschreven worden");
        exit(EXIT_FAILURE);
    }
    return resultaat;
}

int close_db(FILE * f) {
    int resultaat = fclose(f);
    if (resultaat != 0) {
        perror("Bestand kan niet gesloten worden");
        exit(EXIT_FAILURE);
    }
    return resultaat;
}
