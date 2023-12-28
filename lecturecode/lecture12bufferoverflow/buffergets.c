#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 5

int main(int argc, char *argv[])
{
    char B[BUFFER_SIZE]; 
    printf("Type log message:");
    gets(B);   /* read log message from standard input into buffer */
    printf("You entered %s\n", B); 
    printf("Beyond buffer= %s\n", (B+8));  
    return 0;
}