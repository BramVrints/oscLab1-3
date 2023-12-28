#include <pthread.h>
#include <stdio.h>

int limit = 0;

pthread_mutex_t mutex;
pthread_cond_t condvar;
int arrived = 0;


int barrier_init(int n){
 limit = n;
arrived = 0;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condvar, NULL);


}

int wait_for_others(void){

        pthread_mutex_lock(&mutex);
        arrived ++;
        while (arrived != limit)
            pthread_cond_wait(&condvar, &mutex); //wait for others
   
        //last one arrived , free the first one waiting
        pthread_cond_signal(&condvar);

        pthread_mutex_unlock(&mutex);

        return 0;

}

