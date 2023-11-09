
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

/*
 * The real definition of struct list / struct node
 */

struct dplist_node {
    dplist_node_t *prev, *next;
    void *element;
};

struct dplist {
    dplist_node_t *head;

    void *(*element_copy)(void *src_element);

    void (*element_free)(void **element);

    int (*element_compare)(void *x, void *y);
};


dplist_t *dpl_create(// callback functions
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {

    if (*list == NULL) {
        return; //De lijst is al weg: er kan geen geheugen worden vrijgemaakt
    }

    dplist_node_t *current = (*list)->head;
    dplist_node_t *next;

    while (current != NULL) {
        next = current->next;

        if (free_element && (*list)->element_free != NULL) {
            //Als de boolean true is en er is een callback method om het element weg te doen,
            // dan wordt die method uitgevoerd met de inhoud van het element
            (*list)->element_free(&(current->element));
        }
        free(current);
        current = next;
    }
    free(*list);
    *list = NULL;
}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {



    if (list == NULL) {
        return NULL;
    }
    void* testptr = malloc(24);

    dplist_node_t *listNode = (dplist_node_t *) malloc(24);

    if (insert_copy && list->element_copy != NULL) {
        //Als insert_copy true is, en er bestaat een callback method om te kopiëren,
        //dan wordt die method gebruikt om een deep copy van het element te maken en in de lijst te steken
        listNode->element = list->element_copy(element);
    }
    else {
        //Anders wordt er een pointer reference naar het element in de lijst gestoken
        listNode->element = element;
    }

    //Geval 1: de lijst is leeg
    if (list->head == NULL) {
        listNode->prev = NULL;
        listNode->next = NULL;
        list->head = listNode;
    }
    //Geval 2: begin van de lijst
    else if (index <= 0) {
        listNode->prev = NULL;
        listNode->next = list->head;
        list->head->prev = listNode;
        list->head = listNode;
    }
    else {
        dplist_node_t* refAtIndex = dpl_get_reference_at_index(list, index);
        //Geval 3: ongeldige index
        if (refAtIndex == NULL) {
            refAtIndex = list->head;
            while (refAtIndex->next != NULL) {
                refAtIndex = refAtIndex->next;
            }
        }
        //Geval 4: middenin de lijst
        if (index < dpl_size(list)) {
            listNode->prev = refAtIndex->prev;
            listNode->next = refAtIndex;
            refAtIndex->prev->next = listNode;
            refAtIndex->prev = listNode;
        }
        //Geval 5: einde van de lijst
        else {
            assert(refAtIndex->next == NULL);
            listNode->next = NULL;
            listNode->prev = refAtIndex;
            refAtIndex->next = listNode;
        }
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {

    //Geval 1: lijst bestaat niet of lijst is leeg
    if (list == NULL || list->head == NULL) {
        return list;
    }

    //Geval 2: begin van de lijst
    if (index <= 0) {
        dplist_node_t *nodeToRemove = list->head;
        list->head = list->head->next;
        if (list->head != NULL) {
            list->head->prev = NULL;
        }

        if (free_element && list->element_free != NULL) {
            //Als de boolean true is en er bestaat een callback method om het element weg te doen,
            //dan wordt deze method gebruikt met de inhoud van het element
            list->element_free(&(nodeToRemove->element));
        }
        free(nodeToRemove);
    }

    //Geval 3 middenin de lijst
    else {
        dplist_node_t *current = list->head;
        int count = 0;

        while (current != NULL && count < index) {
            current = current->next;
            count++;
        }

        if (current != NULL) {
            if (current->prev != NULL) {
                current->prev->next = current->next;
            }
            if (current->next != NULL) {
                current->next->prev = current->prev;
            }

            if (free_element && list->element_free != NULL) {
                //idem
                list->element_free(&(current->element));
            }
            free(current);
        }

        //Geval 4 ongeldige index: laatste element wordt verwijderd
        else {
            dplist_node_t *end = list->head;
            while (end->next != NULL) {
                end = end->next;
            }
            if (end->prev != NULL) {
                end->prev->next = NULL;
            }
            if (free_element && list->element_free != NULL) {
                //idem
                list->element_free(&(end->element));
            }
            free(end);
        }
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

void *dpl_get_element_at_index(dplist_t *list, int index) {

    //Geval 1: lijst bestaat niet of lijst is leeg
    if (list == NULL || list->head == NULL) {
        return NULL;
    }

    //Geval 2: begin van de lijst of negatieve index
    if (index <= 0) {
        return list->head->element;
    }
    //Geval 3: middenin de lijst
    else {
        dplist_node_t *current = list->head;
        int count = 0;

        while (current != NULL && count < index) {
            current = current->next;
            count++;
        }

        if (current != NULL) {
            return current->element;
        }
        //Geval 4 einde van de lijst of index groter dan size van de lijst
        else {
            dplist_node_t *end = list->head;
            while (end->next != NULL) {
                end = end->next;
            }
            return end->element;
        }
    }
}

int dpl_get_index_of_element(dplist_t *list, void *element) {

    //Lijst bestaat niet of lijst is leeg
    if (list == NULL || list->head == NULL) {
        return -1;
    }

    //Callback functie element_compare is niet geïmplementeerd
    if (list->element_compare == NULL) {
        return -1;
    }

    dplist_node_t *current = list->head;
    int count = 0;

    while (current != NULL) {
        if (list->element_compare(current->element, element) == 0) {
            return count;
        }
        current = current->next;
        count++;
    }
    //Element niet gevonden
    return -1;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {

    //Geval 1: lijst bestaat niet of lijst is leeg
    if (list == NULL || list->head == NULL) {
        return NULL;
    }

    //Geval 2: begin van de lijst of negatieve index
    if (index <= 0) {
        return list->head;
    }
    //Geval 3: middenin de lijst
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
        //Geval 4 einde van de lijst
        else {
            dplist_node_t *end = list->head;
            while (end->next != NULL) {
                end = end->next;
            }
            return end;
        }
    }
}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {

    //Lijst bestaat niet of meegegeven referentie bestaat niet
    if (list == NULL || reference == NULL) {
        return NULL;
    }

    dplist_node_t *current = list->head;

    while (current != NULL) {
        if (current == reference) {
            return current->element;
        }
        current = current->next;
    }
    //Element niet gevonden
    return NULL;
}
