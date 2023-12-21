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

/**
 * Opens the storage manager.
 * @param filename the csvFile to give as parameter to process_sensor_data_from_sbuffer
 * @param append whether there needs to be appended or (over)written
 * @return the csvFile
 */
FILE * open_db(char * filename, bool append);

/**
 * Reads data from sbuffer by using the sbuffer_remove function
 * Subsequentlly calls the insert_sensor function to insert the data it read from sbuffer into csv file
 * @param f the csvFile to give as parameter to insert_sensor
 * @param buffer the shared buffer to read the data from
 */
void process_sensor_data_from_sbuffer(FILE * f, sbuffer_t *buffer);

/**
 * Prints the sensor data that is read in process_sensor_data_from_sbuffer into the csv file
 * @param f the csv file
 * @param id the sensor id
 * @param value the sensor temperature
 * @param ts the sensor timestamp
 * @return 0 if insertion succeeded, -1 if insertion failed
 */
int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts);

/**
 * Closes the storage manager
 * @param f the csv file
 * @return -1 if an error occurred, 0 if the storage manager closed succesfully
 */
int close_db(FILE * f);

#endif /* _SENSOR_DB_H_ */