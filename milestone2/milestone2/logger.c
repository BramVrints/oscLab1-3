//
// Created by bram on 15/11/23.
//
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

static FILE *logFile;

int write_to_log_process(char *msg) {
    fprintf( logFile, "%d - %.*s - %s\n", 1, (int)(strlen(asctime(localtime(&(time_t){time(NULL)}))) -1), asctime(localtime(&(time_t){time(NULL)})), msg);
    return 0;
}

int create_log_process() {
    logFile = fopen("example.gateway.log", "a");
    if (logFile == NULL) {
        printf("Log file kan niet geopend worden");
        return -1;
    }
    else {
        printf("Log file is geopend\n");
        return 0;
    }
}

int end_log_process() {
    return fclose(logFile);
}


