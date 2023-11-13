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

    copy->lastModified = malloc(sizeof(time_t));
    ERROR_HANDLER(copy->lastModified == NULL, "Memory allocation fout in element_copy voor lastModified");
    copy->lastModified = ((my_element_t *)element)->lastModified;


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

    if (sensorList == NULL) {
        sensorList = dpl_create(element_copy, element_free, element_compare);
        ERROR_HANDLER(sensorList == NULL, "Lijst maken lukt niet");
    }

    uint16_t roomId, sensorId;
    //SCNu16 macro om een uint16_t te lezen
    while (fscanf(fp_sensor_map, "%" SCNu16 " %" SCNu16, &roomId, &sensorId) == 2) {

    }
}




