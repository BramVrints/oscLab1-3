#define _GNU_SOURCE



#include "dplist.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct {
    int id;
    char* name;
} my_element_t;

void* element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void * element_copy(void * element) {
    my_element_t* copy = malloc(sizeof (my_element_t));
    char* new_name;
    asprintf(&new_name,"%s",((my_element_t*)element)->name); //asprintf requires _GNU_SOURCE
    assert(copy != NULL);
    copy->id = ((my_element_t*)element)->id;
    copy->name = new_name;
    return (void *) copy;
}

void element_free(void ** element) {
    free((((my_element_t*)*element))->name);
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y) {
    return ((((my_element_t*)x)->id < ((my_element_t*)y)->id) ? -1 : (((my_element_t*)x)->id == ((my_element_t*)y)->id) ? 0 : 1);
}

void ck_assert_msg(bool result, char * msg){
    if(!result) printf("%s\n", msg);
}

void yourtest1() {
    // Test free NULL, don't use callback
    dplist_t *list = NULL;
    dpl_free(&list, false);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // Test free NULL, use callback
    list = NULL;
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // Test free empty list, don't use callback
    list = dpl_create(element_copy, element_free, element_compare);
    dpl_free(&list, false);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // Test free empty list, use callback
    list = dpl_create(element_copy, element_free, element_compare);
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    //Test free list met 1 element, zonder callback
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t e1 = {1, "Jos"};
    dpl_insert_at_index(list, &e1, 0, false);
    dpl_free(&list, false);
    ck_assert_msg(list == NULL, "Resultaat moet NULL zijn");

    //Test free list met 1 element, met callback
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t e2 = {2, "Danny"};
    dpl_insert_at_index(list, &e2, 0, false);
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Resultaat moet NULL zijn");

    //Test free list met meerdere elementen, zonder callback
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t e3 = {3, "Geoffrey"};
    my_element_t e4 = {4, "Erwin"};
    dpl_insert_at_index(list, &e3, 0, false);
    dpl_insert_at_index(list, &e4, 1, false);
    dpl_free(&list, false);
    ck_assert_msg(list == NULL, "Resultaat moet NULL zijn");

    //Test free list met meerdere elementen, met callback
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t e5 = {5, "Michiel"};
    my_element_t e6 = {6, "Kathleen"};
    dpl_insert_at_index(list, &e5, 0, false);
    dpl_insert_at_index(list, &e6, 1, false);
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Resultaat moet NULL zijn");
}

void test02_create() {
    //test case 1: lijst met callback functions maken
    dplist_t *list1 = dpl_create(element_copy, element_free, element_compare);
    ck_assert_msg(list1 != NULL, "Resultaat mag niet NULL zijn");

    //test case 2: lijst zonder callback functions maken
    dplist_t *list2 = dpl_create(NULL, NULL, NULL);
    ck_assert_msg(list2 != NULL, "Resultaat mag niet NULL zijn");

    //test case 3: lijst met een paar callback functions maken
    dplist_t *list3 = dpl_create(element_copy, NULL, element_compare);
    ck_assert_msg(list3 != NULL, "Resultaat mag niet NULL zijn");


    dpl_free(&list1, true);
    dpl_free(&list2, true);
    dpl_free(&list3, true);
}

int test03_insert_at_index() {
    //test case 1: begin van de lijst
    dplist_t *list1 = dpl_create(element_copy, element_free, element_compare);
    my_element_t e1 = {1, "Koen Pelsmaekers"};
    dpl_insert_at_index(list1, &e1, 0, false);
    ck_assert_msg(element_compare(dpl_get_element_at_index(list1, 0), &e1) == 0,
                  "element bij index 0 moet 'Koen Pelsmaekers' zijn");
    dpl_free(&list1, true);

    //test case 2: einde van de lijst
    dplist_t *list2 = dpl_create(element_copy, element_free, element_compare);
    my_element_t e2 = {2, "Koen Eneman"};
    dpl_insert_at_index(list2, &e2, 0, false);
    ck_assert_msg(element_compare(dpl_get_element_at_index(list2, 0), &e2) == 0,
                  "element bij index 0 moet 'Koen Eneman' zijn");
    my_element_t e3 = {3, "Ludo Bruynseels"};
    //Hier wordt het element aan het einde van de lijst toegevoegd
    dpl_insert_at_index(list2, &e3, 0, false);
    ck_assert_msg(element_compare(dpl_get_element_at_index(list2, 1), &e3) == 0,
                  "element bij index 1 moet 'Ludo Bruynseels' zijn");
    dpl_free(&list2, true);

    //test case 3: middenin de lijst
    dplist_t *list3 = dpl_create(element_copy, element_free, element_compare);
    my_element_t e4 = {4, "Toon van Waterschoot"};
    dpl_insert_at_index(list3, &e4, 0, false);
    ck_assert_msg(element_compare(dpl_get_element_at_index(list3, 0), &e4) == 0,
                  "element bij index 0 moet 'Toon van Waterschoot' zijn");
    my_element_t e5 = {5, "Jeroen van Aken"};
    dpl_insert_at_index(list3, &e5, 1, false);
    ck_assert_msg(element_compare(dpl_get_element_at_index(list3, 1), &e5) == 0,
                  "element bij index 1 moet 'Jeroen van Aken' zijn");
    my_element_t e6 = {6, "Stijn Langendries"};
    //Hier wordt het element echt in het midden toegevoegd
    dpl_insert_at_index(list3, &e6, 1, false);
    ck_assert_msg(element_compare(dpl_get_element_at_index(list3, 1), &e6) == 0,
                  "element bij index 1 moet 'Stijn Langendries' zijn, want het wordt in het midden gestoken");
    dpl_free(&list3, true);

    return 0;
}




int main(void) {

    yourtest1();
    test02_create();
    test03_insert_at_index();


    return 0;
}
