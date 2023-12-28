#include <stdio.h>
#include "myheader.h"
#include "grandfather.h"
#include "father.h"

#define MAX_ARRAY_LENGTH 20
#undef FILE_SIZE
#define FILE_SIZE 42
#ifndef MESSAGE
#define MESSAGE "You wish!"
#endif
#ifdef DEBUG
 /* Your debugging statements here */
#endif
#define square(x) ((x) * (x))
#define message_for(a, b) \
 printf(#a " and " #b ": Hello!\n")
int main(void) {
     int a = square(MY_SIZE);
     message_for(Carole, Debra); //not a function call !
     return 0;
}
