//
// Created by bram on 7/12/23.
//
#include <string.h>
#include <unistd.h>
#include "connmgr.h"
#include "sbuffer.h"
#include "datamgr.h"

int pipeInt[2];

int main(int argc, char *argv[]) {

    //Dit is de testcode voor data manager

    // Initialize the sbuffer and datamgr
    sbuffer_t *buffer;
    if (sbuffer_init(&buffer) != SBUFFER_SUCCESS) {
        fprintf(stderr, "Error initializing sbuffer\n");
        return 1;
    }

    // Insert sample sensor data into the buffer (replace with actual data)
    sensor_data_t data1;
    data1.id = 1; // Sensor ID
    data1.value = 25.5; // Sensor value
    data1.ts = 1638822000; // Timestamp

    // Insert data into the buffer (you can do this in a loop to simulate multiple data points)
    if (sbuffer_insert(buffer, &data1) != SBUFFER_SUCCESS) {
        fprintf(stderr, "Error inserting data into the buffer\n");
        return 1;
    }

    // Perform some datamgr operations (e.g., get room ID, get average, etc.)
    sensor_id_t sensor_id = 1; // Replace with a valid sensor ID
    uint16_t room_id = datamgr_get_room_id(sensor_id);
    sensor_value_t avg = datamgr_get_avg(sensor_id);
    time_t last_modified = datamgr_get_last_modified(sensor_id);

    // Output the results
    printf("Room ID: %\n", room_id);
    printf("Average: %.2lf\n", avg);
    printf("Last Modified: %ld\n", (long)last_modified);

    // Cleanup
    sbuffer_free(&buffer);






    return 0;
}

void write_to_log_process(char *msg) {
    write(pipeInt[1], msg, strlen(msg));
}

