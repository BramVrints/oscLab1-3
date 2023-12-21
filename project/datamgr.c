//
// Created by bram on 12/11/23.
//
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datamgr.h"
#include "lib/dplist.h"
#include "sbuffer.h"
#include "config.h"

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
    copy->runningAvg = (double *) malloc(RUN_AVG_LENGTH * sizeof(double));
    ERROR_HANDLER(copy->runningAvg == NULL, "Memory allocation fout");
    for (int i = 0; i < RUN_AVG_LENGTH; i++) {
    	copy->runningAvg[i] = ((my_element_t *)element)->runningAvg[i];
    }
    copy->insertedData = ((my_element_t *)element)->insertedData;
    copy->lastModified = ((my_element_t *)element)->lastModified;

    return (void *)copy;
}

void element_free(void **element)
{
    free(*element);
    *element = NULL;
}

int element_compare(void *x, void *y)
{
    sensor_id_t xid = ((my_element_t *)x)->sensorId;
    sensor_id_t yid = ((my_element_t *)y)->sensorId;
    sensor_id_t  diff = xid - yid;
    if (diff != 0) { diff = -1;}
    return diff;
}

void datamgr_parse_sensor_files(FILE *fp_sensor_map, sbuffer_t *buffer) {

    ERROR_HANDLER(fp_sensor_map == NULL || buffer == NULL, "Invalid parameters");

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

            //opkuisen
            free(newSensor->runningAvg);
            free(newSensor);
        }
    }

    //we gaan sensor data uitlezen en de gegevens bij de bijhorende sensoren in de lijst steken
    //dit moet uit de buffer lezen, niet meer uit een file
    sensor_id_t fileSensorId;
    sensor_value_t temperature;
    sensor_ts_t timestamp;
    sensor_data_t *data;
    data = (sensor_data_t *) malloc(sizeof(sensor_data_t ));

    while (1) {
        int result = sbuffer_peek(buffer, data);

        if (result == SBUFFER_NO_DATA) {
            break;
        }

        if (result == SBUFFER_SUCCESS) {
            fileSensorId = data->id;
            temperature = data->value;
            timestamp = data->ts;
            /*printf("datamgr.c: Sensor id %d \n", fileSensorId);
            printf("datamgr.c: Temperature id %lf \n", temperature);
            printf("datamgr.c: Timestamp id %ld \n", timestamp);*/

            //De code om het in de lijst te steken en de average te berekenen is hetzelfde gebleven
            my_element_t elem;
            elem.sensorId = fileSensorId;
            char msg[MAX_STR_LEN];
            int index = dpl_get_index_of_element(sensorList, &elem);
            if (index != -1) {
                my_element_t *currentSensor = dpl_get_element_at_index(sensorList, index);
                if (currentSensor->insertedData == RUN_AVG_LENGTH) {
                    for (int i = 0; i < RUN_AVG_LENGTH - 1; i++) {
                        currentSensor->runningAvg[i] = currentSensor->runningAvg[i + 1];
                    }
                    currentSensor->runningAvg[RUN_AVG_LENGTH - 1] = temperature;
                } else {
                    currentSensor->runningAvg[currentSensor->insertedData] = temperature;
                    currentSensor->insertedData += 1;
                }
                currentSensor->lastModified = timestamp;

                //loggen voor te warm of te koud
                double avg = datamgr_get_avg(currentSensor->sensorId);
                if (avg > SET_MAX_TEMP) {
                    snprintf(msg, 255, "Sensor node %d reports it's too hot (avg temp = %lf).", currentSensor->sensorId, avg);
                    write_to_log_process(msg);
                }
                else if (avg < SET_MIN_TEMP) {
                    snprintf(msg, 255, "Sensor node %d reports it's too cold (avg temp = %lf).", currentSensor->sensorId, avg);
                    write_to_log_process(msg);
                }
            }
            else {
                //printf("datamgr.c: Fout bij uitlezen steken van de data in de lijst\n");
                sprintf(msg, "Received sensor data with invalid sensor node ID %d", data->id);
                write_to_log_process(msg);
            }
        }
        else {
            exit(EXIT_FAILURE);
        }
    }
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id) {
    ERROR_HANDLER(sensor_id < 0, "Ongeldige sensor ID" );
    int index = dpl_get_index_of_element(sensorList, &sensor_id);
    ERROR_HANDLER(index == -1, "Sensor ID niet gevonden in de lijst");
    my_element_t *sensor = dpl_get_element_at_index(sensorList, index);
    return sensor->roomId;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id) {
    ERROR_HANDLER(sensor_id < 0, "Ongeldige sensor ID");
    my_element_t data;
    data.sensorId = sensor_id;
    int index = dpl_get_index_of_element(sensorList, &data);
    ERROR_HANDLER(index == -1, "Sensor ID niet gevonden in de lijst");
    my_element_t *sensor = dpl_get_element_at_index(sensorList, index);

    sensor_value_t sum = 0;
    int count = (sensor->insertedData < RUN_AVG_LENGTH) ? sensor->insertedData : RUN_AVG_LENGTH;

    for (int i = 0; i < count; i++) {
        sum += sensor->runningAvg[i];
    }
    return (count > 0) ? (sum / count) : 0.0;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id) {
    ERROR_HANDLER(sensor_id < 0, "Ongeldige sensor ID" );
    int index = dpl_get_index_of_element(sensorList, &sensor_id);
    ERROR_HANDLER(index == -1, "Sensor ID niet gevonden in de lijst");
    my_element_t *sensor = dpl_get_element_at_index(sensorList, index);
    return sensor->lastModified;
}

int datamgr_get_total_sensors() {
    return dpl_size(sensorList);
}
