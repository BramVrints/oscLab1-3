//
// Created by bram on 7/12/23.
//
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include "connmgr.h"
#include "sbuffer.h"
#include "datamgr.h"
#include "sensor_db.h"
#include "config.h"

#define READ_END 0
#define WRITE_END 1

char wmsg[MAX_STR_LEN];
int logPipe[2];

sbuffer_t *buffer;
pthread_t processThreads[3];
pthread_attr_t processThreadsAttr;
pthread_mutex_t pipeMutex;
int counter = 0;

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
        printf("Please provide the right arguments: first the port, then the max nb of clients\n");
        return -1;
    }
    MAX_CONN = atoi(argv[2]);
    PORT = atoi(argv[1]);


    if (pipe(logPipe) == -1) {
        perror("Pipe maken voor logger lukt niet");
    }

    pid_t loggerPid = fork();
    if (loggerPid < 0) {
        fprintf(stderr, "Fork mislukt");
        return -1;
    }

    //CHILD PROCESS:
    if (loggerPid == 0) {
        FILE *logFile = fopen("gateway.log", "w");
        close(logPipe[WRITE_END]);
        char character;
        while (1) {
            //de pipe byte per byte uitlezen
            read(logPipe[0], &character, 1);
            if (character == '$') {
                break;
            }
            fwrite(&character, 1, 1, logFile);
            fflush(logFile);
        }
        close(logPipe[READ_END]);
        fclose(logFile);
        return 0;
    }

    //PARENT PROCESS:
    else {
        // initialize de buffer en al de rest
        if (sbuffer_init(&buffer) != SBUFFER_SUCCESS) {
            fprintf(stderr, "Error initializing sbuffer\n");
            return -1;
        }
        close(logPipe[READ_END]);
        pthread_attr_init(&processThreadsAttr);
        pthread_mutex_init(&pipeMutex, NULL);

        //threads starten
        pthread_create(&processThreads[0], &processThreadsAttr, startconnmgr, NULL);
        pthread_create(&processThreads[1], &processThreadsAttr, startdatamgr, NULL);
        pthread_create(&processThreads[2], &processThreadsAttr, startstoragemgr, NULL);
        printf("main.c: Threads created\n");

        //threads joinen
        pthread_join(processThreads[0],NULL);
        printf("main.c: Connection manager joined\n");
        pthread_join(processThreads[1],NULL);
        printf("main.c: Data manager joined\n");
        pthread_join(processThreads[2], NULL);
        printf("main.c: Storage manager joined\n");

        //aangeven aan logger dat het gedaan is met end of log character
        write(logPipe[WRITE_END], "$", 1);
        close(logPipe[WRITE_END]);

        //opkuisen
        wait(NULL);
        sbuffer_free(&buffer);
        printf("main.c: Buffer gefreet, programma sluit af\n");
        return 0;
    }
}

int write_to_log_process(char *msg) {
    //kritische sectie
    pthread_mutex_lock(&pipeMutex);
    //Deze code komt uit de slides van de feedback van milestone 2
    time_t tijd = time(NULL);
    char *tijdString = ctime(&tijd);
    tijdString[strlen(tijdString)-1] = '\0';
    snprintf(wmsg, MAX_STR_LEN, "%d - %s - %s\n", counter++, tijdString, msg);
    long pipeResult = write(logPipe[WRITE_END], wmsg, strlen(wmsg));
    //einde kritische sectie
    pthread_mutex_unlock(&pipeMutex);
    if (pipeResult == -1) {
        return -1;
    }
    return 0;
}
