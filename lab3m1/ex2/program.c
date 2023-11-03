#define _GNU_SOURCE
/**
 * \author Bert Lagaisse
 *
 * main method that executes some test functions (without check framework)
 */

#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "dplist.h"

void ck_assert_msg(bool result, char * msg){
    if(!result) printf("%s\n", msg);
}

dplist_t *makeSampleList() {
    dplist_t *list = dpl_create();
    dpl_insert_at_index(list, "Element1", 0);
    dpl_insert_at_index(list, "Element2", 1);
    dpl_insert_at_index(list, "Element3", 2);
    dpl_insert_at_index(list, "Element4", 3);
    dpl_insert_at_index(list, "Element5", 4);
    return list;
}

void test01_create(){
    dplist_t *strings = NULL;
    strings = dpl_create();
    ck_assert_msg(strings != NULL, "numbers = NULL, List not created");
    ck_assert_msg(dpl_size(strings) == 0, "Numbers may not contain elements.");
    dpl_free(&strings);
}

int test02_insert_at_index(){
    /*// Lege lijst maken
    dplist_t *list = dpl_create();

    // Test Case 1: Insert at the beginning of the list
    dpl_insert_at_index(list, "World", 0);
    assert(strcmp(list->head->element, "World") == 0);

    // Test Case 2: Insert at the end of the list
    dpl_insert_at_index(list, "Hello", 1);
    assert(strcmp(list->tail->element, "Hello") == 0);

    // Test Case 3: Insert in the middle of the list
    dpl_insert_at_index(list, "Middle", 1);
    assert(strcmp(list->head->next->element, "Middle") == 0);

    // Free the list and its elements
    dpl_free(list);*/
    return 0;
}

void test03_remove_at_index() {
    dplist_t *list = makeSampleList();

    // Test Case 1: Remove the head node (index 0)
    dpl_remove_at_index(list, 0);
    // Test Case 2: Remove a node in the middle (index 2)
    dpl_remove_at_index(list, 2);
    // Test Case 3: Remove the last node (index 2)
    dpl_remove_at_index(list, 2);

    //Lijst printen om visueel te verifiëren of de functie zijn werk doet
    dplist_node_t *current = dpl_get_element_at_index(list,0);
    int count = 1;
    while (current != NULL) {
        printf("%s\n", current);
        current = dpl_get_element_at_index(list,count);
        count++;
    }
    dpl_free(list);
}

int test04_size() {
    dplist_t *list = makeSampleList();

    //Test case 1: get the size van the lijst
    int size = dpl_size(list);
    assert(size == 5);

    //Test case 2: get the size van een lege lijst
    dpl_free(list);
    list = dpl_create();
    size = dpl_size(list);
    assert(size == 0);

    //Test case 3 get the size van een invalid (NULL) lijst
    size = dpl_size(NULL);
    assert(size == -1);

    dpl_free(list);
    return 0;
}


int test05_get_reference_at_index() {
    dplist_t *list = makeSampleList();

    //test case 1: index 0
    dplist_node_t *ref_1 = dpl_get_reference_at_index(list, 0);
    assert(strcmp(ref_1->element, "Element1") == 0);

    //test case 2: middenin de lijst
    dplist_node_t *ref_2 = dpl_get_reference_at_index(list, 2);
    assert(strcmp(ref_2->element, "Element3") == 0);

    //test case 3: index buiten de lijst (negatieve index)
    dplist_node_t *ref_3 = dpl_get_reference_at_index(list, -1);
    assert(ref_3 == NULL);

    //test case 4: index buiten de lijst (te grote index)
    dplist_node_t *ref_4 = dpl_get_reference_at_index(list, 5);
    assert(ref_4 == dpl_get_element_at_index(list, dpl_size(list) - 1));

    //test case 5: lege lijst
    dpl_free(list);
    list = dpl_create();
    dplist_node_t *ref_5 = dpl_get_reference_at_index(list, 0);
    assert(ref_5 = NULL);


    dpl_free(list);
    return 0;
}

int test06_get_element_at_index() {
    dplist_t *list = makeSampleList();

    //test case 1: normaal scenario
    element_t e1 = dpl_get_element_at_index(list,2);
    assert(strcmp(e1, "Element3") == 0);

    //test case 2: index is groter dan de size van de lijst
    element_t e2 = dpl_get_element_at_index(list,5);
    assert(strcmp(e2, "Element5") == 0);

    //test case 3: index is niet geldig
    element_t e3 = dpl_get_element_at_index(list,-1);
    assert(e3 == 0);

    dpl_free(list);
    return 0;
}

int test07_get_index_of_element() {
    dplist_t *list = makeSampleList();

    //test case 1: normaal scenario
    int index1 = dpl_get_index_of_element(list, "Element2");
    assert(index1 == 1);

    //test case 2: element dat niet in de lijst zit
    int index2 = dpl_get_index_of_element(list, "Element420");
    assert(index2 == -1);

    //test case 3: lijst is leeg
    dpl_free(list);
    list = dpl_create();
    int index3 = dpl_get_index_of_element(list,"Element1");
    assert(index3 == -1);

    dpl_free(list);
    return 0;
}


int main(void)
{
    test01_create();
    test02_insert_at_index();
    test03_remove_at_index();
    test04_size();
    test05_get_reference_at_index();
    test06_get_element_at_index();
    test07_get_index_of_element();


    return 0;
}