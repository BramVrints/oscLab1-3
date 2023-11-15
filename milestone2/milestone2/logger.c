//
// Created by bram on 15/11/23.
//
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <wait.h>

static int pipe_fd[2];
static FILE *logFile;

int write_to_log_process(char *msg) {
    ssize_t bytesWritten = write(pipe_fd[1], msg, strlen(msg));
    if (bytesWritten == -1) {
        perror("Error writing to the log process");
        return -1;
    }
    return 0;
}

int create_log_process() {
    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation mislukt");
        return -1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return -1;
    }

    if (pid == 0) {
        close(pipe_fd[1]);

        logFile = fopen("gateway.log","w");
        if (logFile == NULL) {
            perror("Log file kan niet geopend worden");
            exit(EXIT_FAILURE);
        }

        char buffer[512];
        ssize_t bytesRead;

        //Berichten van de pipe lezen en naar de log file schrijven:
        while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
            buffer[bytesRead] = '\0';
            fprintf(logFile, "%s", buffer);
        }
        close(pipe_fd[0]);
        fclose(logFile);
        exit(EXIT_SUCCESS);
    }

    close(pipe_fd[0]);
    return 0;
}

int end_log_process() {
    close(pipe_fd[1]);
    wait(NULL);
    return 0;
}


