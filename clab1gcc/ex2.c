#include <stdio.h>
#include "ex2.h"
#include <limits.h>

void printFileTypeSize() {
	
	printf("Size of int: %zu bytes\n", sizeof(int));
	printf("Size of float: %zu bytes\n", sizeof(float));
	printf("Size of double: %zu bytes\n", sizeof(double));
	printf("Size of void: %zu bytes\n", sizeof(void));
	int *ptr;
	printf("Size of pointer: %zu bytes\n", sizeof(ptr));
	printf("Size of short: %zu bytes\n", sizeof(short));
	printf("Size of long: %zu bytes\n", sizeof(long));
	
	if (CHAR_MIN < 0) {
		printf("Char is signed on this system.\n");
	} else {
		printf("Char is unsigned on this system.\n");
		}

	return;
}
