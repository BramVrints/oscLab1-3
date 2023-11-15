//
// Created by bram on 15/11/23.
//
#define SIZE 25
#define READ_END 0
#define WRITE_END 1

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main() {
    char wmsg[SIZE] = "Greetings";
    char rmsg[SIZE];
    pid_t pid;
    int fd[2];

    if (pipe(fd) == -1) {
        printf("Pipe failed");
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        printf("Fork failed");
        return 1;
    }
    else if (pid == 0) {
        close(fd[WRITE_END]);
        read(fd[READ_END], rmsg, SIZE);
        for (int i = 0; i< strlen(rmsg); i++) {
            char a = rmsg[i];
            if (a >= 'a' && a <= 'z') {
                rmsg[i] = toupper(a);
            }
            else {
                rmsg[i] = tolower(a);
            }
        }
        printf("Child read %s\n", rmsg);
        close(fd[READ_END]);
    }
    return 0;
}