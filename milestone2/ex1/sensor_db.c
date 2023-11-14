//
// Created by bram on 14/11/23.
//
#include "sensor_db.h"
#include <inttypes.h>

FILE * open_db(char * filename, bool append) {
    FILE  *file = fopen(filename, append ? "a" : "w");
    if (file == NULL) {
        perror("Bestand kan niet geopend worden");
        exit(EXIT_FAILURE);
    }
    return file;
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
