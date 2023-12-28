#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 5
int main(int argc, char *argv[])
{
	char buffer[BUFFER_SIZE];
	if (argc < 2)
		return -1;
	else {
		strcpy(buffer,argv[1]);
        printf("buffer content= %s\n", buffer);
        printf("Beyond buffer= %s\n", (buffer+8));     
		return 0;
	}
}
