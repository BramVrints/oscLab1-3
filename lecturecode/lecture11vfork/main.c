#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <stdio.h>
int main()
{
    int n = 10;
    pid_t pid = vfork();
    if (pid == 0) // child process
    {
        n = 9;
        execlp("/bin/ls", "ls", NULL);
        printf("Child process started\n");
    }
    else // parent process execution
    {
        //wait(NULL);
        printf("back in parent process\n");
    }
    printf("value of n: %d \n", n);
    return 0;
}
