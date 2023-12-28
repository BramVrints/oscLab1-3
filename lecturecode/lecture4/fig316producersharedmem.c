
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

int main()
{
    const int SIZE = 4096; //size of the shared mem object
    const char *name = "OS"; //name of the shared mem object
    const char *msg0 = "Coding in C";
    const char *msg1 = " on Linux";
    const char *msg2 = " is a bit challenging!\n";

    int shm_fd; //filedescriptor
    char *ptr; //return type of malloc and mmap

    //create shared mem segment
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    //config size of shared mem segment
    ftruncate(shm_fd, SIZE);

    ptr = (char*) mmap(0, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);


    if (ptr == MAP_FAILED){
        printf("Map failed\n");
        return -1;
    }
   
    sprintf(ptr, "%s", msg0);
    ptr += strlen(msg0);

    sprintf(ptr,"%s", msg1);
    ptr += strlen(msg1) +1;

    sprintf(ptr,"%s", msg2);
    ptr += strlen(msg2);

    return 0;

}