
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1


int main(void)
{
    char write_msg[BUFFER_SIZE] = "Greetings";
    char read_msg[BUFFER_SIZE];
    pid_t pid;
    int fd[2];

    // create the pipe
    if (pipe(fd) == -1)
    {
        printf("Pipe failed\n");
        return 1;
    }

    // fork the child
    pid = fork();

    if (pid < 0)
    { // fork error
        printf("fork failed\n");
        return 1;
    }

    if (pid > 0)
    { // parent process
        close(fd[READ_END]);
        write(fd[WRITE_END], write_msg, strlen(write_msg) + 1);
        close(fd[WRITE_END]);
        printf("parent waits...\n");
        wait(NULL);
        printf("parent done.\n");
    }
    else
    { // child process
        close(fd[WRITE_END]);
        read(fd[READ_END], read_msg, BUFFER_SIZE);
        printf("Child reads %s\n", read_msg);
        close(fd[READ_END]);
    }

    return 0;
}