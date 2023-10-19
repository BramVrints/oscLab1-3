#include <stdio.h>

void swap_pointers(void **a, void **b) {
	//temporary pointer to swap the values of two void pointers
	void *temp = *a;
	*a = *b;
	*b = temp;
}

int main() {
    int a = 1;
    int b = 2;
    int *p = &a;
    int *q = &b;

    printf("address of p = %p and q = %p\n", (void *)p, (void *)q);

    swap_pointers((void **)&p, (void **)&q);

    printf("address of p = %p and q = %p\n", (void *)p, (void *)q);

    return 0;
}
