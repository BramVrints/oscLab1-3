/**
 * \author Bert Lagaisse
 */

#ifndef _SENSOR_DB_H_
#define _SENSOR_DB_H_

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "sbuffer.h"
#include <stdbool.h>

FILE * open_db(char * filename, bool append);

/**
 * Reads data from sbuffer by using the sbuffer_remove function
 * Subsequentlly calls the insert_sensor function to insert the data it read from sbuffer into csv file
 * @param f the csvFile to give as parameter to insert_sensor
 * @param buffer the shared buffer to read the data from
 */
void process_sensor_data_from_sbuffer(FILE * f, sbuffer_t *buffer);

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts);

int close_db(FILE * f);

#endif /* _SENSOR_DB_H_ */