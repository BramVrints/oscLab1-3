#include <stdio.h>
#include <stdlib.h>

typedef struct {
    short day, month;
    unsigned year;
} date_t;

date_t *date_struct(int day, int month, int year) {
    date_t *dummy = (date_t *)malloc(sizeof(date_t)); // Allocate memory on the heap
    if (dummy == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    dummy->day = (short)day;
    dummy->month = (short)month;
    dummy->year = (unsigned)year;
    return dummy;
}

void cleanup_date(date_t *date) {
    free(date); // Free the dynamically allocated memory
}

int main(void) {
    int day, month, year;
    date_t *d;
    printf("\nGive day, month, year:");
    scanf("%d %d %d", &day, &month, &year);
    d = date_struct(day, month, year);

    // Experiment with memory leaks using Valgrind
    // Remember to compile with debugging information (-g flag) for accurate Valgrind results
    // Use 'valgrind ./your_program' to check for memory leaks

    // Cleanup at the end
    cleanup_date(d);

    return 0;
}
