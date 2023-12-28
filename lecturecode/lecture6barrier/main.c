/**
 * Script  for testing the barrier implementation.
 *
 * Usage:
 *	./testbarrier <number of threads.
 */

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "barrier.h"

void *worker(void *worker)
{
    int sleep_time;

    /* sleep for a random period of time */
    sleep_time = (int) ( (random() % MAX_SLEEP_TIME) + 1);
    printf("sleeping for %d seconds\n",sleep_time);
    sleep(sleep_time);
    printf("slept for %d seconds\n",sleep_time);
    /* now wait for other threads to arrive */
    wait_for_others(); //this is a barrier.
    printf("out of the barrier\n");

    return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr,"Usage: ./testbarrier <number of threads>\n");
		return -1;
	}

	int i;
	int number = atoi(argv[1]);

	pthread_t workers[number];

	if ( barrier_init(atoi(argv[1])) != 0)
		return -1;

	for (i = 0; i < number; i++) {
		pthread_create(&workers[i], 0, worker, 0);
	}

	for (i = 0; i < number; i++)
		pthread_join(workers[i], 0);

	return 0;
}
