/**
 * \author Jeroen Van Aken, Bert Lagaisse, Ludo Bruynseels
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "dplist.h"

//String duplicate functie
char *strdup(const char *src) {
    if (src == NULL) {
        return NULL;
    }
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (dest != NULL) {
        strcpy(dest, src);
    }
    return dest;
}

typedef char *element_t;
/*
 * The real definition of struct list / struct node
 */
struct dplist_node {
    dplist_node_t *prev, *next;
    element_t element; //Het element is een char *
};

struct dplist {
    dplist_node_t *head;
    dplist_node_t *tail;
    // more fields will be added later
};

dplist_t *dpl_create() {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    list->head = NULL;
    list->tail = NULL;
  return list;
}

void dpl_free(dplist_t **list) {

    //Do extensive testing with valgrind.
    if (list == NULL) {
        return;
    }

    dplist_node_t *current = (*list)->head;
    while (current != NULL) {
        dplist_node_t *next = current->next;
        //Zowel de node als de string die erbij hoort worden vrij gemaakt
        free(current->element);
        free(current);
        current = next;
    }

    free(list);
}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/


dplist_t *dpl_insert_at_index(dplist_t *list, element_t element, int index) {

    if (list == NULL || element == NULL) {
        return list;
    }

    //Geheugen alloceren voor de nieuwe node
    dplist_node_t *new_node = (dplist_node_t *)malloc(sizeof(dplist_node_t));
    if (new_node == NULL) {
        return list;
    }

    //Element (string) kopiëren met eerder gedeclareerde string duplicate functie
    new_node->element = strdup(element);

    if (list->head == NULL) {
        //Geval 1 lijst is leeg
        new_node->prev = NULL;
        new_node->next = NULL;
        list->head = new_node;
        list->tail = new_node;
    }
    else if (index <= 0) {
        //Geval 2 begin van de lijst
        new_node->prev = NULL;
        new_node->next = list->head;
        list->head->prev = new_node;
        list->head = new_node;
    }
    else {
        dplist_node_t *ref_at_index = dpl_get_reference_at_index(list, index);
        if (ref_at_index != NULL) {
            //Geval 3 middenin de lijst
            new_node->prev = ref_at_index->prev;
            new_node->next = ref_at_index;
            if (ref_at_index->prev != NULL) {
                ref_at_index->prev->next = new_node;
            }
            else {
                list->head = new_node;
            }
            ref_at_index->prev = new_node;
        }
        else {
            //Geval 4 einde van de lijst
            new_node->next = NULL;
            new_node->prev = list->tail;
            list->tail->next = new_node;
            list->tail = new_node;
        }
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index) {

    //Geval 1: lijst bestaat niet of lijst is leeg
    if (list == NULL || dpl_size(list) == 0 || index < 0) {
        return list;
    }

    //Geval 2 begin van de lijst
    if (index == 0) {
        dplist_node_t *nodeToRemove = list->head;

        if (dpl_size(list) == 1) {
            list->head = NULL;
            list->tail = NULL;
        }
        else {
            list->head = list->head->next;
            list->head->prev = NULL;
        }
        free(nodeToRemove->element);
        free(nodeToRemove);
    }
    //Geval 3 niet begin van de lijst
    else {
        dplist_node_t *nodeToRemove = dpl_get_reference_at_index(list, index);

        if (nodeToRemove == NULL) {
            return list; //dan is de index fout
        }
        else if (nodeToRemove == list->tail) {
            list->tail = list->tail->prev;
            list->tail->next = NULL;
        }
        else {
            nodeToRemove->prev->next = nodeToRemove->next;
            nodeToRemove->next->prev = nodeToRemove->prev;
        }
        free(nodeToRemove->element);
        free(nodeToRemove);
    }

    return list;
}

int dpl_size(dplist_t *list) {
    if (list == NULL) {
        return -1;
    }
    int size = 0;
    dplist_node_t *current = list->head;
    while (current != NULL) {
        current = current->next;
        size++;
    }
    return size;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    if (list == NULL || list->head == NULL || index < 0) {
        return NULL;
    }

    dplist_node_t *current = list->head;
    int count = 0;

    while (current != NULL && count < index) {
        current = current->next;
        count++;
    }

    if (current != NULL) {
        return current;
    }
    else {
        return list->tail;
    }
}

element_t dpl_get_element_at_index(dplist_t *list, int index) {

    if (list == NULL || list->head == NULL || index < 0) {
        return 0;
    }

    dplist_node_t *current = list->head;
    int count = 0;

    while (current != NULL && count < index) {
        current = current->next;
        count++;
    }

    if (current != NULL) {
        return current->element;
    }
    else {
        return 0;
    }
}

int dpl_get_index_of_element(dplist_t *list, element_t element) {

    if (list == NULL || list->head == NULL || element == NULL) {
        return -1;
    }

    dplist_node_t *current = list->head;
    int count = 0;

    while (current != NULL) {
        if (current->element == element) {
            return count;
        }
        current = current->next;
        count++;
    }
    //element zit niet in de lijst
    return -1;
}
