#define _GNU_SOURCE
/**
 * \author Bert Lagaisse
 *
 * main method that executes some test functions (without check framework)
 */

#include <stdio.h>
#include "dplist.h"

void ck_assert_msg(bool result, char * msg){
    if(!result) printf("%s\n", msg);
}

void test01_create(){
    dplist_t *numbers = NULL;
    numbers = dpl_create();
    ck_assert_msg(numbers != NULL, "numbers = NULL, List not created");
    ck_assert_msg(dpl_size(numbers) == 0, "Numbers may not contain elements.");
    dpl_free(&numbers);
}

void test02_insert_at_index(){
    dplist_t *numbers = NULL;
    numbers = dpl_create();

    dpl_insert_at_index(numbers, 'a', 0);
    ck_assert_msg(dpl_size(numbers) == 1, "Numbers must contain 1 element.");

    dpl_insert_at_index(numbers, '\e', -1);
    ck_assert_msg(dpl_size(numbers) == 2, "Numbers must contain 2 elements.");

    dpl_insert_at_index(numbers, 'b', 4);
    ck_assert_msg(dpl_size(numbers) == 3, "Numbers must contain 3 elements.");

    dpl_free(&numbers);
}

void test03_remove_at_index() {
    dplist_t *numbers = NULL;
    numbers = dpl_create();

    //1 Lijst is nog leeg
    dpl_remove_at_index(numbers, 'a');
    ck_assert_msg(dpl_size(numbers) == 0, "Numbers must have size 0.");

    //2 Begin van de lijst
    dpl_insert_at_index(numbers, 'a', 0);
    dpl_remove_at_index(numbers, 'a');
    ck_assert_msg(dpl_size(numbers) == 0, "Numbers must have size 0.");

    //3 Einde van de lijst
    dpl_insert_at_index(numbers, 'a', 0);
    dpl_insert_at_index(numbers, 'b', 2);
    dpl_remove_at_index(numbers, 'b');
    ck_assert_msg(dpl_size(numbers) == 1, "Numbers must have size 1.");

    //4 Middenin de lijst
    dpl_insert_at_index(numbers, 'a', 0);
    dpl_insert_at_index(numbers, 'b', 2);
    dpl_insert_at_index(numbers, 'c',4);
    dpl_remove_at_index(numbers, 'b');
    ck_assert_msg(dpl_size(numbers) == 2, "Numbers must have size 2.");

    dpl_free(&numbers);
}

void test04_size() {
    dplist_t *numbers = NULL;
    numbers = dpl_create();

    dpl_insert_at_index(numbers, 'a', 0);
    dpl_insert_at_index(numbers, 'b', 2);
    dpl_insert_at_index(numbers, 'c',4);
    ck_assert_msg(dpl_size(numbers) == 2, "Numbers must have size 3.");

    dpl_free(&numbers);
}

void test05_get_reference_at_index() {
    dplist_t *numbers = NULL;
    numbers = dpl_create();
    dplist_node_t *ref;

    ck_assert_msg(dpl_get_reference_at_index(numbers,0) == NULL,"Empty list returns NULL");

    dpl_insert_at_index(numbers, 'a', 0);
    dpl_insert_at_index(numbers, 'b', 2);
    dpl_insert_at_index(numbers, 'c',4);

    ref = dpl_get_reference_at_index(numbers,0);
    ck_assert_msg(ref == dpl_get_reference_at_index(numbers,0),"Begin van de lijst");
    ref = dpl_get_reference_at_index(numbers,2);
    ck_assert_msg(dpl_get_reference_at_index(numbers, 2) == ref,"Middenin de lijst");
    ref = dpl_get_reference_at_index(numbers,4);
    ck_assert_msg(dpl_get_reference_at_index(numbers, 4) == ref,"Einde van de lijst");

    dpl_free(&numbers);
}

void test06_get_element_at_index() {
    dplist_t *numbers = NULL;
    numbers = dpl_create();

    ck_assert_msg(dpl_get_element_at_index(numbers,0) == NULL,"Empty list returns NULL");

    dpl_insert_at_index(numbers, 'a', 0);
    dpl_insert_at_index(numbers, 'b', 2);
    dpl_insert_at_index(numbers, 'c',4);

    ck_assert_msg(dpl_get_element_at_index(numbers,0) == 'a',"Begin van de lijst");
    ck_assert_msg(dpl_get_element_at_index(numbers, 2) == 'b',"Middenin de lijst");
    ck_assert_msg(dpl_get_element_at_index(numbers, 4) == 'c',"Einde van de lijst");

    dpl_free(&numbers);
}



int main(void)
{
    test01_create();
    test02_insert_at_index();
    test03_remove_at_index();
    test04_size();
    test05_get_reference_at_index();
    test06_get_element_at_index();


    return 0;
}