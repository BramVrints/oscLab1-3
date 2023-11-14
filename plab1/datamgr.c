//
// Created by bram on 12/11/23.
//
//#include <stdint-gcc.h>
//#include <bits/types/time_t.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "datamgr.h"
#include "lib/dplist.h"

dplist_t *sensorList;

typedef struct
{
    sensor_id_t roomId;
    sensor_id_t sensorId;
    sensor_value_t *runningAvg;
    uint16_t insertedData;
    sensor_ts_t lastModified;
} my_element_t;

//De callback functions uit de testcode van milestone 1 worden aangepast om te werken met dit soort element

void *element_copy(void *element)
{
    my_element_t *copy = malloc(sizeof(my_element_t));
    ERROR_HANDLER(copy == NULL, "Memory allocation fout in element_copy functie");

    copy->roomId = ((my_element_t *)element)->roomId;
    copy->sensorId = ((my_element_t *)element)->sensorId;
    copy->runningAvg = ((my_element_t *)element)->runningAvg;
    copy->lastModified = ((my_element_t *)element)->lastModified;

    //Dynamic memory allocation voor time_t type?
    //copy->lastModified = malloc(sizeof(time_t));
    //ERROR_HANDLER(copy->lastModified == NULL, "Memory allocation fout in element_copy voor lastModified");
    //copy->lastModified = ((my_element_t *)element)->lastModified;

    return (void *)copy;
}

void element_free(void **element)
{
    //free(((my_element_t *)*element)->name);
    free(*element);
    *element = NULL;
}

int element_compare(void *x, void *y)
{
    //we gaan roomId vergelijken
    if (((my_element_t *)x)->roomId < ((my_element_t*)y)->roomId) { return -1;}
    else if (((my_element_t *)x)->roomId > ((my_element_t *)y)->roomId) { return -1;}

    else {
        //als roomid hetzelfde is, gaan we kijken naar sensorid
        if (((my_element_t *)x)->sensorId < ((my_element_t *)y)->sensorId) { return -1;}
        else if (((my_element_t *)x)->sensorId > ((my_element_t *)y)->sensorId) { return 1;}
        else { return 0;}
    }
}


void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data) {

    ERROR_HANDLER(fp_sensor_map == NULL || fp_sensor_data == NULL, "Invalid parameters");

    //Als de lijst nog niet bestaat, maken we ze aan
    if (sensorList == NULL) {
        sensorList = dpl_create(element_copy, element_free, element_compare);
        ERROR_HANDLER(sensorList == NULL, "Lijst maken lukt niet");
    }

    char line[256];

    //we gaan sensor map uitlezen, de sensoren maken en in de lijst steken
    while (fgets(line, sizeof(line), fp_sensor_map) != NULL) {
        uint16_t roomId, sensorId;
        //SCNu16 macro om een uint16_t te lezen
        if (sscanf(line, "%" SCNu16 " %" SCNu16, &roomId, &sensorId) == 2) {
            if (sensorList == NULL) {
                ERROR_HANDLER(true, "Probleem! De lijst is niet geïnitialiseerd");
            }

            my_element_t *newSensor = malloc(sizeof(my_element_t ));
            ERROR_HANDLER(newSensor == NULL, "Memory allocation error in datamgr_parse_sensor_files");

            newSensor->sensorId = sensorId;
            newSensor->roomId = roomId;
            newSensor->runningAvg = (double *) malloc(RUN_AVG_LENGTH * sizeof(double));
            ERROR_HANDLER(newSensor->runningAvg == NULL, "Memory allocation error");
            for (int i =0; i< RUN_AVG_LENGTH; i++) {
                newSensor->runningAvg[i] = 0.0;
            }
            newSensor->insertedData = 0;
            newSensor->lastModified = 0;
            dpl_insert_at_index(sensorList, newSensor, dpl_size(sensorList), true);
            free(newSensor->runningAvg);
            free(newSensor);
        }
    }


    //we gaan sensor data uitlezen en de gegevens bij de bijhorende sensoren in de lijst steken
    sensor_id_t fileSensorId;
    sensor_value_t temperature;
    sensor_ts_t timestamp;
    while (fread(&fileSensorId, sizeof(fileSensorId), 1, fp_sensor_data) == 1 &&
            fread(&temperature, sizeof(temperature), 1, fp_sensor_data) == 1 &&
            fread(&timestamp, sizeof(timestamp), 1, fp_sensor_data) == 1) {
        int index = dpl_get_index_of_element(sensorList, &fileSensorId);
        if (index != -1) {
            my_element_t *currentSensor = dpl_get_element_at_index(sensorList, index);
            if (currentSensor->insertedData == RUN_AVG_LENGTH) {
                for (int i = 0; i < RUN_AVG_LENGTH - 1; i++) {
                    currentSensor->runningAvg[i] = currentSensor->runningAvg[i+1];
                }
                currentSensor->runningAvg[RUN_AVG_LENGTH-1] = temperature;
            }
            else {
                currentSensor->runningAvg[currentSensor->insertedData] = temperature;
                currentSensor->insertedData += 1;
            }
            currentSensor->lastModified = timestamp;
        }
        else {
            fprintf(stderr, "Oei: sensor id " PRIu16 " niet gevonden in de sensor map\n", fileSensorId);
        }
    }
}

/*void update_running_average(my_element_t *sensor, double newTemperature) {
    //De geschiedenis van de temperatuur wordt opgeslagen in een array van doubles:
    static double temperatureHistory[RUN_AVG_LENGTH] = {0.0};
    static int historyIndex = 0;

    //De huidige (=nieuwste) temperatuur wordt aan de array toegevoegd:
    temperatureHistory[historyIndex] = newTemperature;
    //Als de array vol zit zal hij terug aan het begin beginnen:
    historyIndex = (historyIndex+1) % RUN_AVG_LENGTH;

    //Het gemiddelde van de geschiedenis van de temperatuur (dus van de array) wordt berekend:
    double sum = 0.0;
    for (int i = 0; i < RUN_AVG_LENGTH; i++) {
        sum += temperatureHistory[i];
    }
    sensor->runningAvg = sum / RUN_AVG_LENGTH;
}*/




