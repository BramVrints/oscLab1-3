/**
 * \author {Bram Vrints}
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "sbuffer.h"

pthread_cond_t cond_var;

/**
 * basic node for the buffer, these nodes are linked together to create the buffer
 */
typedef struct sbuffer_node {
    struct sbuffer_node *next;  /**< a pointer to the next node*/
    sensor_data_t data;         /**< a structure containing the data */
} sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t *head;       /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;       /**< a pointer to the last node in the buffer */
    pthread_mutex_t mutex;
};

int sbuffer_init(sbuffer_t **buffer) {
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    //Mutex initialiseren:
    pthread_mutex_init(&(*buffer)->mutex, NULL);
    //POSIX condition variable initialiseren:
    pthread_cond_init(&cond_var, NULL);
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) {
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }
    //Buffer kapot maken
    pthread_mutex_destroy(&(*buffer)->mutex);
    //Nodes vd buffer leegmaken
    while ((*buffer)->head) {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    free(*buffer);
    *buffer = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data) {
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;

    //Kritische sectie
    pthread_mutex_lock(&buffer->mutex);

    //Als de buffer leeg is gaat het niet, dus moeten we terug unlocken
    while (buffer->head == NULL) {
        pthread_cond_wait(&cond_var, &buffer->mutex);
        //return SBUFFER_NO_DATA;
    }

    *data = buffer->head->data;

    //Als het de end of stream character is:
    if (data->id == 0) {
        pthread_cond_signal(&cond_var);
        pthread_mutex_unlock(&buffer->mutex);
        return SBUFFER_SUCCESS;
    }

    dummy = buffer->head;
    if (buffer->head == buffer->tail) // buffer has only one node
    {
        buffer->head = buffer->tail = NULL;
    } else  // buffer has many nodes empty
    {
        buffer->head = buffer->head->next;
    }

    //Einde kritische sectie
    pthread_mutex_unlock(&buffer->mutex);
    free(dummy);
    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) {
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;

    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) {
        //pthread_mutex_unlock(&buffer->mutex);
        return SBUFFER_FAILURE;
    }

    dummy->data = *data;
    dummy->next = NULL;

    //kritische sectie
    pthread_mutex_lock(&buffer->mutex);

    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }

    pthread_cond_signal(&cond_var);
    //kritische sectie terug vrijgeven
    pthread_mutex_unlock(&buffer->mutex);
    return SBUFFER_SUCCESS;
}
