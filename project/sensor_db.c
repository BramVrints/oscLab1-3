//
// Created by bram on 14/11/23.
//
#include "sensor_db.h"
#include "sbuffer.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

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
}

void process_sensor_data_from_sbuffer(FILE * f, sbuffer_t *buffer) {
    sensor_data_t data;
    int result;

    while (1) {
        result = sbuffer_remove(buffer, &data);
        if (result == SBUFFER_NO_DATA) {
            break;
        }

        if (result == SBUFFER_SUCCESS) {
            if(insert_sensor(f, data.id, data.value, data.ts) != 0) {
                fprintf(stderr, "Error inserting sensor data into csv file.\n");
            }
        }

        else {
            fprintf(stderr, "Error receiving data from sbuffer.\n");
            //printf("sensor_db.c: Fout bij uitlezen steken van de data in de lijst\n");
        }
    }
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    if (f == NULL) {
        write_to_log_process("An error occurred when writing to the csv file.");
        return -1;
    }

    //Zo wordt de data in de csv file geprint:
    int resultaat = fprintf(f, "%" PRIu16 ",%g,%" PRIu64 "\n", id, value, (uint64_t)ts);

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
