//
// Created by bram on 7/12/23.
//
#include <string.h>
#include <unistd.h>
#include "connmgr.h"
#include "sbuffer.h"
#include "datamgr.h"
#include "sensor_db.h"

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
void * startstoragemgr() {
    FILE *csvFile = open_db("data.csv", false);
    process_sensor_data_from_sbuffer(csvFile, buffer);
    close_db(csvFile);
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
    pthread_create(&processThreads[2], &processThreadsAttr, startstoragemgr, NULL);

    printf("main.c: Threads created\n");

    pthread_join(processThreads[0],NULL);
    printf("main.c: Connection manager joined\n");
    pthread_join(processThreads[1],NULL);
    printf("main.c: Data manager joined\n");
    pthread_join(processThreads[2], NULL);
    printf("main.c: Storage manager joined\n");


    // Cleanup
    sbuffer_free(&buffer);
    printf("main.c: Buffer gefreet, programma sluit af\n");
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

