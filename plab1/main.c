#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "lib/dplist.h"
#include "datamgr.h"
#include <time.h>
#include <inttypes.h>

int main(){
    printf("Hello World\n");

    FILE * map = fopen("room_sensor.map", "r");
    FILE * data = fopen("sensor_data", "rb");

    if(map == NULL) return -1;
    if(data == NULL) return -1;

    datamgr_parse_sensor_files(map, data);

    datamgr_free();

    fclose(map);
    fclose(data);

    int total_sensors = datamgr_get_total_sensors();
    for (int i = 0; i < total_sensors; i++) {
        sensor_id_t sensor_id = datamgr_get_room_id(i);
        sensor_value_t avg = datamgr_get_avg(sensor_id);
        time_t last_modified = datamgr_get_last_modified(sensor_id);
        printf("Sensor ID: %" PRIu16 ", Room ID: %" PRIu16 ", AVG: %f, Last Modified: %ld\n",
                sensor_id, datamgr_get_room_id(sensor_id), avg, last_modified);
    }

    return 0;
}