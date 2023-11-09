#define _GNU_SOURCE

#include "dplist.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

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
    my_element_t *e1 = malloc(sizeof(my_element_t));
    e1->id = 1;
    e1->name = "Jos";
    dpl_insert_at_index(list, e1, 0, false);
    dpl_free(&list, false);
    ck_assert_msg(list == NULL, "Resultaat moet NULL zijn");

    //Test free list met 1 element, met callback
    list = dpl_create(element_copy, element_free, element_compare);
    //Geheugen op heap vrijmaken voor het element
    my_element_t *e2 = malloc(sizeof(my_element_t));
    e2->id = 2;
    //Geheugen op heap vrijmaken voor de naam van het element
    e2->name = malloc(strlen("Danny")+1);
    //De naam op de heap zetten
    strcpy(e2->name, "Danny");
    dpl_insert_at_index(list, e2, 0, false);
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Resultaat moet NULL zijn");

    //Test free list met meerdere elementen, zonder callback
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *e3 = malloc(sizeof(my_element_t));
    e3->id = 3;
    e3->name = "Geoffrey";
    my_element_t *e4 = malloc(sizeof(my_element_t));
    e4->id = 4;
    e4->name = "Erwin";
    dpl_insert_at_index(list, e3, 0, false);
    dpl_insert_at_index(list, e4, 1, false);
    dpl_free(&list, false);
    ck_assert_msg(list == NULL, "Resultaat moet NULL zijn");

    //Test free list met meerdere elementen, met callback
    list = dpl_create(element_copy, element_free, element_compare);
    //Plaats vrijmaken op heap voor het element
    my_element_t *e5 = malloc(sizeof(my_element_t));
    e5->id = 5;
    //Plaats vrijmaken voor de naam van het element
    e5->name = malloc(strlen("Michiel")+1);
    //De naam op de heap zetten
    strcpy(e5->name, "Michiel");
    //Plaats vrijmaken op heap voor het element
    my_element_t *e6 = malloc(sizeof(my_element_t));
    e6->id = 6;
    //Plaats vrijmaken voor de naam van het element
    e6->name = malloc(strlen("Kathleen")+1);
    //de naam op de heap zetten
    strcpy(e6->name, "Kathleen");
    dpl_insert_at_index(list, e5, 0, false);
    dpl_insert_at_index(list, e6, 1, false);
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
    my_element_t *e1 = malloc(sizeof(my_element_t));
    e1->id = 1;
    e1->name = malloc(strlen("Koen Pelsmaekers")+1);
    strcpy(e1->name, "Koen Pelsmaekers");
    dpl_insert_at_index(list1, e1, 0, false);
    ck_assert_msg(element_compare(dpl_get_element_at_index(list1, 0), e1) == 0,
                  "element bij index 0 moet 'Koen Pelsmaekers' zijn");
    dpl_free(&list1, true);

    //test case 2: einde van de lijst
    dplist_t *list2 = dpl_create(element_copy, element_free, element_compare);
    my_element_t *e2 = malloc(sizeof(my_element_t));
    e2->id = 2;
    e2->name = malloc(strlen("Koen Eneman")+1);
    strcpy(e2->name, "Koen Eneman");
    dpl_insert_at_index(list2, e2, 0, false);
    ck_assert_msg(element_compare(dpl_get_element_at_index(list2, 0), e2) == 0,
                  "element bij index 0 moet 'Koen Eneman' zijn");
    my_element_t *e3 = malloc(sizeof(my_element_t));
    e3->id = 3;
    e3->name = malloc(strlen("Ludo Bruynseels")+1);
    strcpy(e3->name, "Ludo Bruynseels");
    //Hier wordt het element aan het einde van de lijst toegevoegd
    dpl_insert_at_index(list2, e3, 1, false);
    ck_assert_msg(element_compare(dpl_get_element_at_index(list2, 1), e3) == 0,
                  "element bij index 1 moet 'Ludo Bruynseels' zijn");
    dpl_free(&list2, true);

    //test case 3: middenin de lijst
    dplist_t *list3 = dpl_create(element_copy, element_free, element_compare);
    my_element_t *e4 = malloc(sizeof(my_element_t));
    e4->id = 4;
    e4->name = malloc(strlen("Toon van Waterschoot")+1);
    strcpy(e4->name, "Toon van Waterschoot");
    dpl_insert_at_index(list3, e4, 0, false);
    ck_assert_msg(element_compare(dpl_get_element_at_index(list3, 0), e4) == 0,
                  "element bij index 0 moet 'Toon van Waterschoot' zijn");
    my_element_t *e5 = malloc(sizeof(my_element_t));
    e5->id = 5;
    e5->name = malloc(strlen("Jeroen van Aken")+1);
    strcpy(e5->name, "Jeroen van Aken");
    dpl_insert_at_index(list3, e5, 1, false);
    ck_assert_msg(element_compare(dpl_get_element_at_index(list3, 1), e5) == 0,
                  "element bij index 1 moet 'Jeroen van Aken' zijn");
    my_element_t *e6 = malloc(sizeof(my_element_t));
    e6->id = 6;
    e6->name = malloc(strlen("Stijn Langendries")+1);
    strcpy(e6->name, "Stijn Langendries");
    //Hier wordt het element echt in het midden toegevoegd
    dpl_insert_at_index(list3, e6, 1, false);
    ck_assert_msg(element_compare(dpl_get_element_at_index(list3, 1), e6) == 0,
                  "element bij index 1 moet 'Stijn Langendries' zijn, want het wordt in het midden gestoken");
    dpl_free(&list3, true);

    return 0;
}

int test04_remove_at_index() {
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);

    //test case 1: als je iets van een lege lijst verwijdert, gaat het niet
    dpl_remove_at_index(list, 0, true);
    assert(dpl_size(list) == 0);

    //test case 2: iets van een lege lijst verwijderen, zonder element free
    dpl_remove_at_index(list, 0, false);
    assert(dpl_size(list) == 0);

    //we gooien een paar dingen/mensen in de lijst
    my_element_t e1 = {1, "Jasper"};
    my_element_t e2 = {2, "Axelle"};
    my_element_t e3 = {3, "Filip"};
    my_element_t e4 = {4, "Jens"};
    dpl_insert_at_index(list, &e1, 0, true);
    dpl_insert_at_index(list, &e2, 1, true);
    dpl_insert_at_index(list, &e3, 2, true);
    dpl_insert_at_index(list, &e4, 3, true);

    //test case 3: als we nu Jasper eruit gooien (met free_element), zitten er nog 3 elementen in
    dpl_remove_at_index(list, 0, true);
    assert(dpl_size(list) == 3);
    //het eerste element is weg dus moet -1 returnen als we de index ervan opvragen
    assert(dpl_get_index_of_element(list, &e1) == -1);

    //test case 4: als we nu de middenste eruit gooien (zonder free_element), zitten er nog 2 elementen in
    dpl_remove_at_index(list, 1, false);
    assert(dpl_size(list) == 2);
    //idem
    assert(dpl_get_index_of_element(list, &e3) == -1);

    dpl_free(&list, true);
    return 0;
}

int test05_size() {
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);

    //test case 1: lege lijst
    assert(dpl_size(list) == 0);

    //Test case 2: lijst met 1 element
    my_element_t e1 = {1, "Windows"};
    dpl_insert_at_index(list, &e1, 0, true);
    assert(dpl_size(list) == 1);

    //test case 3: lijst met meerdere elementen
    my_element_t e2 = {2, "Linux"};
    dpl_insert_at_index(list, &e2, 1, true);
    my_element_t e3 = {3, "MacOS"};
    dpl_insert_at_index(list, &e3, 2, true);
    assert(dpl_size(list) == 3);

    //test case 4: lijst is leeg als je alles eruit gooit
    dpl_remove_at_index(list, 0, true);
    dpl_remove_at_index(list, 0, true);
    dpl_remove_at_index(list, 0, true);
    assert(dpl_size(list) == 0);

    dpl_free(&list, true);
    return 0;
}

int test06_get_reference_at_index() {
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    dpl_insert_at_index(list, "Element1", 0, false);
    dpl_insert_at_index(list, "Element2", 1, false);
    dpl_insert_at_index(list, "Element3", 2, false);

    //test case 1: index 0
    dplist_node_t *ref_1 = dpl_get_reference_at_index(list, 0);
    //dan assert om te kijken of het opgehaalde element overeenkomt met element1

    //test case 2: middenin de lijst
    dplist_node_t *ref_2 = dpl_get_reference_at_index(list, 2);
    //dan assert om te kijken of het opgehaalde element overeenkomt met element1

    //test case 3: index buiten de lijst (negatieve index)
    dplist_node_t *ref_3 = dpl_get_reference_at_index(list, -1);
    assert(ref_3 == NULL);

    //test case 4: index buiten de lijst (te grote index)
    dplist_node_t *ref_4 = dpl_get_reference_at_index(list, 5);
    assert(ref_4 == dpl_get_element_at_index(list, dpl_size(list) - 1));

    //test case 5: lege lijst
    dpl_free(&list, true);
    list = dpl_create(element_copy, element_free, element_compare);
    dplist_node_t *ref_5 = dpl_get_reference_at_index(list, 0);
    assert(ref_5 == NULL);

    dpl_free(&list, true);
    return 0;
}

int test07_get_element_at_index() {
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    dpl_insert_at_index(list, "Element1", 0, false);
    dpl_insert_at_index(list, "Element2", 1, false);
    dpl_insert_at_index(list, "Element3", 2, false);

    //test case 1: index 0
    void *e1 = dpl_get_element_at_index(list,0);
    assert(((char *)e1)[7] == '1');

    //test case 2: middenin de lijst
    void *e2 = dpl_get_element_at_index(list,1);
    assert(((char *)e1)[7] == '2');

    //test case 3: index -1: het eerste element wordt meegegeven
    void *e3 = dpl_get_element_at_index(list,-1);
    assert(((char *)e1)[7] == '1');

    //test case 4: index die buiten de lijst valt: het laatste element wordt meegegeven
    void *e4 = dpl_get_element_at_index(list, 5);
    assert(((char *)e1)[7] == '3');

    //test case 5: lege lijst
    dpl_free(&list, true);
    list = dpl_create(element_copy, element_free, element_compare);
    void *e5 = dpl_get_element_at_index(list, 0);
    assert(e5 == NULL);

    dpl_free(&list, true);
    return 0;
}

int test08_get_index_of_element() {
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    dpl_insert_at_index(list, "Element1", 0, false);
    dpl_insert_at_index(list, "Element2", 1, false);
    dpl_insert_at_index(list, "Element3", 2, false);

    //test case 1: zoeken naar een bestaand element
    char *gezochtElement1 = "Element2";
    int index1 = dpl_get_index_of_element(list, gezochtElement1);
    assert(index1 == 1);

    //test case 2: zoeken naar een niet-bestaand element
    char *gezochtElement2 = "Element420";
    int index2 = dpl_get_index_of_element(list, gezochtElement2);
    assert(index2 == -1);

    //test case 3: zoeken naar een element in een lege lijst
    dpl_free(&list, true);
    list = dpl_create(element_copy, element_free, element_compare);
    char *gezochtElement3 = "Element";
    int index3 = dpl_get_index_of_element(list, gezochtElement3);
    assert(index3 == -1);

    //test case 4: lijst=null
    dplist_t *list2 = NULL;
    char *gezochtElement4 = "Element";
    int index4 = dpl_get_index_of_element(list2, gezochtElement4);
    //assert om te kijken of NULL gereturned wordt

    dpl_free(&list, true);
    dpl_free(&list2, true);
    return 0;
}

int test09_get_element_at_reference() {
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    dpl_insert_at_index(list, "Element1", 0, false);
    dpl_insert_at_index(list, "Element2", 1, false);
    dpl_insert_at_index(list, "Element3", 2, false);

    //test case 1: begin van de lijst
    dplist_node_t *ref1 = dpl_get_reference_at_index(list, 0);
    my_element_t *e1 = dpl_get_element_at_reference(list, ref1);
    assert(e1 != NULL);
    assert(e1->id == 1);
    assert(((char *)e1)[7] == '1');

    //test case 2: middenin de lijst
    dplist_node_t *ref2 = dpl_get_reference_at_index(list, 1);
    my_element_t *e2 = dpl_get_element_at_reference(list, ref2);
    assert(e2 != NULL);
    assert(e2->id == 2);
    assert(((char *)e2)[7] == '2');

    //test case 3: einde van de lijst
    dplist_node_t *ref3 = dpl_get_reference_at_index(list, 2);
    my_element_t *e3 = dpl_get_element_at_reference(list, ref3);
    assert(e3 != NULL);
    assert(e3->id == 3);
    assert(((char *)e3)[7] == '3');

    //test case 4: reference is helaas NULL
    dplist_node_t *ref4 = NULL; //wat jammer
    my_element_t *e4 = dpl_get_element_at_reference(list, ref4);
    assert(e4 == NULL);

    //test case 5: reference is geen bestaande reference in de lijst
    dplist_node_t *ref5 = malloc(dpl_size(dpl_create(element_copy, element_free, element_compare)));
    my_element_t *e5 = dpl_get_element_at_reference(list, ref5);
    assert(e5 == NULL);
    free(ref5);

    //test case 6: lege lijst
    dpl_free(&list, true);
    list = dpl_create(element_copy, element_free, element_compare);
    dplist_node_t *ref6 = dpl_get_reference_at_index(list, 0);
    my_element_t *e6 = dpl_get_element_at_reference(list, ref6);
    assert(e6 == NULL);

    dpl_free(&list, true);
    return 0;
}


int main(void) {

    yourtest1();
    test02_create();
    test03_insert_at_index();
//    test04_remove_at_index();
//    test05_size();
//    test06_get_reference_at_index();
//    test07_get_element_at_index();
//    test08_get_index_of_element();
//    test09_get_element_at_reference();


    return 0;
}
