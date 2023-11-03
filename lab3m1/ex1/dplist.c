/**
 * \author Jeroen Van Aken, Bert Lagaisse, Ludo Bruynseels
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "dplist.h"


/*
 * The real definition of struct list / struct node
 */
struct dplist_node {
    dplist_node_t *prev, *next;
    element_t element;
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
  return list;
}

void dpl_free(dplist_t **list) {

    //Do extensive testing with valgrind.
    if (*list == NULL) {
        return;
    }

    dplist_node_t *current = (*list)->head;
    while (current != NULL) {
        dplist_node_t *next = current->next;
        free(current);
        current = next;
    }

    free(*list);
    *list = NULL;
}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/


dplist_t *dpl_insert_at_index(dplist_t *list, element_t element, int index) {
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));

    list_node->element = element;
    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index) {

    //Geval 1: lijst bestaat niet of lijst is leeg
    if (list == NULL || list->head == NULL) {
        return list;
    }

    dplist_node_t *nodeToRemove;
    if (index <= 0) {
        //Geval 2: begin van de lijst
        nodeToRemove = list->head;
        list->head = list->head->next;
        if (list->head == NULL) {
            //In dit geval zit er maar 1 element in de lijst, dus wordt de lijst leeg
            list->tail = NULL;
        }
        else {
            list->head->prev = NULL;
        }
    }
    else {
        dplist_node_t *refAtIndex = dpl_get_reference_at_index(list, index);
        if (refAtIndex == NULL) {
            //Geval 3: einde van de lijst (d.w.z. index >= size)
            nodeToRemove = list->tail;
            list->tail = list->tail->prev;
            if (list->tail == NULL) {
                //In dit geval zit er maar 1 element in de lijst, dus wordt de lijst leeg
                list->head = NULL;
            } else {
                list->tail->next = NULL;
            }
        } else {
            //Geval 4: in het midden van de lijst
            nodeToRemove = refAtIndex; //(refAtIndex verwijst naar het element dat verwijderd moet worden
            nodeToRemove->prev->next = nodeToRemove->next;
            nodeToRemove->next->prev = nodeToRemove->prev;
        }
    }

    free(nodeToRemove);
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

    if (list==NULL || list->head == NULL) {
        return NULL;
    }

    if (index <= 0) {
        return list->head;
    }
    else {
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
            dplist_node_t *end = list->head;
            while (end->next != NULL) {
                end = end->next;
            }
            return end;
        }
    }
}

element_t dpl_get_element_at_index(dplist_t *list, int index) {

    int count = 0;
    dplist_node_t *current = list->head;
    //Geval 1: lijst bestaat niet of lijst is leeg
    if (list == NULL || list->head == NULL) {
        return 0;
    }
    //Geval 2: Begin van de lijst
    if (index <= 0) {
        return list->head->element;
    }
    //Geval 3: middenin de lijst
    while (current != NULL) {
        if (count == index) {
            return current->element;
        }
        current = current->next;
        count++;
    }
    //geval 4: einde van de lijst
    return list->tail->element;
}

int dpl_get_index_of_element(dplist_t *list, element_t element) {

    int count =0;
    dplist_node_t *current = list->head;
    //geval 1 lijst bestaat niet
    if (list == NULL) {
        return -1;
    }
    //geval 2 element zit ergens in de lijst
    while (current != NULL) {
        if (current->element == element) {
            return count;
        }
        current = current->next;
        count++;
    }
    //geval 3 element zit niet in de lijst
    return -1;
}
