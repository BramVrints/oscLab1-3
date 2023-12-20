//
// Created by bram on 7/12/23.
//
#include <string.h>
#include <unistd.h>
#include "connmgr.h"
#include "sbuffer.h"
#include "datamgr.h"

int pipeInt[2];
sbuffer_t *buffer;

pthread_t processThreads[3];
pthread_attr_t processThreadsAttr;

int MAX_CONN;
int PORT;

void * startconnmgr() {
    connmgr_main(MAX_CONN, PORT, buffer);
    pthread_exit(NULL);
}
void * startdatamgr() {
    FILE *map = fopen("room_sensor.map", "r");
    datamgr_parse_sensor_files(map, buffer);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    if(argc < 3) {
        printf("Please provide the right arguments: first the port, then the max nb of clients");
        return -1;
    }
    MAX_CONN = atoi(argv[2]);
    PORT = atoi(argv[1]);

    // initialize de buffer
    if (sbuffer_init(&buffer) != SBUFFER_SUCCESS) {
        fprintf(stderr, "Error initializing sbuffer\n");
        return 1;
    }

    //threads starten
    pthread_create(&processThreads[0], &processThreadsAttr, startconnmgr, NULL);
    pthread_create(&processThreads[1], &processThreadsAttr, startdatamgr, NULL);

    pthread_join(processThreads[0],NULL);
    pthread_join(processThreads[1],NULL);


    //testcode voor datamgr
    // Insert sample sensor data into the buffer (replace with actual data)
   /* sensor_data_t data1;
    data1.id = 15; // Sensor ID
    data1.value = 25.5; // Sensor value
    data1.ts = 1638822000; // Timestamp*/

    // Insert data into the buffer (you can do this in a loop to simulate multiple data points)
    /*if (sbuffer_insert(buffer, &data1) != SBUFFER_SUCCESS) {
        fprintf(stderr, "Error inserting data into the buffer\n");
        return 1;
    }*/
    /*
    // Perform some datamgr operations (e.g., get room ID, get average, etc.)
    sensor_id_t sensor_id = 15; // Replace with a valid sensor ID
    uint16_t room_id = datamgr_get_room_id(sensor_id);
    sensor_value_t avg = datamgr_get_avg(sensor_id);
    time_t last_modified = datamgr_get_last_modified(sensor_id);*/

    /*// Output the results
    //printf("Room ID: %\n", room_id);
    printf("Average: %.2lf\n", avg);
    printf("Last Modified: %ld\n", (long)last_modified);*/




    // Cleanup
    sbuffer_free(&buffer);
    return 0;
}

void write_to_log_process(char *msg) {
    write(pipeInt[1], msg, strlen(msg));
}

void datamgrtest() {
    for (int i=0; i<2; i++) {
        sensor_data_t *data = malloc(sizeof(sensor_data_t ));
        data->id = 15;
        data->value = 10+i;
        data->ts = time(NULL);
        sbuffer_insert(buffer, data);
        sleep(1);
        free(data);
    }
    sensor_data_t data;
    data.id = 0;
    sbuffer_insert(buffer, &data);
}

