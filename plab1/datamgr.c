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


typedef struct
{
    uint16_t roomId;
    uint16_t sensorId;
    double runningAvg;
    time_t lastModified;
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

static dplist_t *sensorList = NULL;

void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data) {

    ERROR_HANDLER(fp_sensor_map == NULL || fp_sensor_data == NULL, "Invalid parameters");

    //Als de lijst nog niet bestaat, maken we ze aan
    if (sensorList == NULL) {
        sensorList = dpl_create(element_copy, element_free, element_compare);
        ERROR_HANDLER(sensorList == NULL, "Lijst maken lukt niet");
    }

    //we gaan sensor map uitlezen, de sensoren maken en in de lijst steken
    uint16_t roomId, sensorId;
    //SCNu16 macro om een uint16_t te lezen
    while (fscanf(fp_sensor_map, "%" SCNu16 " %" SCNu16, &roomId, &sensorId) == 2) {
        if (sensorList == NULL) {
            ERROR_HANDLER(true, "Probleem! De lijst is niet geïnitialiseerd");
        }

        my_element_t *newSensor = malloc(sizeof(my_element_t ));
        ERROR_HANDLER(newSensor == NULL, "Memory allocation error in datamgr_parse_sensor_files");

        newSensor->sensorId = sensorId;
        newSensor->roomId = roomId;
        newSensor->runningAvg = 0.0;
        newSensor->lastModified = 0;
        dpl_insert_at_index(sensorList, newSensor, dpl_size(sensorList), false);
    }

    //we gaan sensor data uitlezen en de gegevens bij de bijhorende sensoren in de lijst steken
    uint16_t fileSensorId;
    double temperature;
    sensor_ts_t timestamp;
    while (fread(&fileSensorId, sizeof(fileSensorId), 1, fp_sensor_data) == 1 &&
            fread(&temperature, sizeof(temperature), 1, fp_sensor_data) == 1 &&
            fread(&timestamp, sizeof(timestamp), 1, fp_sensor_data) == 1) {
        int index = dpl_get_index_of_element(sensorList, &fileSensorId);
        if (index != -1) {
            my_element_t *currentSensor = dpl_get_element_at_index(sensorList, index);
            //update_running_average(currentSensor, temperature);
            currentSensor->lastModified = timestamp;
        }
        else {
            fprintf(stderr, "Oei: sensor id " PRIu16 " niet gevonden in de sensor map\n", fileSensorId);
        }
    }


    dpl_free(&sensorList, true);
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




