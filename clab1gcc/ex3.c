#include <stdio.h>
#include <string.h>
#include "ex3.h"
#define MAX 100

int isDigit(char ch) {
    return ch >= '0' && ch <= '9';
}

int isLowerCase(char ch) {
    return ch >= 'a' && ch <= 'z';
}

int toUpperCase(char ch) {
    if (isLowerCase(ch)) {
        return ch - 'a' + 'A';
    }
    return ch;
}

void declareStrings() {

	//initialize strings
	char first[MAX], second[MAX], str[MAX], name[MAX];
	
	//read first and second name
	printf("Enter your first name: ");
	scanf("%s", first);
	printf("Enter your second name: ");
	scanf("%s", second);
	
	//convert second name to uppercase and store it in str
	int i = 0;
	while (second[i] != '\0') {
        str[i] = toUpperCase(second[i]);
        i++;
        }
        str[i] = '\0';
        printf("First Name: %s\n", first);
        printf("Second Name (Uppercase): %s\n", str);

	//strcmp
	int result_strcmp = strcmp(second, str);
	printf("Result of strcmp: %d\n", result_strcmp);
	
	//concatenate first and second name into name
	strcpy(name, first);
	strcat(name, " ");
	strcat(name, second);
	printf("Full name: %s\n", name);
	
	//read birth year into int
	int year;
	printf("Enter your birth year: ");
	scanf("%d", &year);
	printf("Your birth year is: %d\n", year);
	
	//concatenate first second and year into name
	int required_length = snprintf(NULL, 0, "%s %s %d", first, second, year);
	snprintf(name, required_length + 1, "%s %s %d", first, second, year);
	printf("Full name is: %s\n", name);
	
	//use sscanf to read from name
	sscanf(name, "%s %s %d", first, second, &year);
	printf("Extracted first name: %s\n", first);
	printf("Extracted second name: %s\n", second);
	printf("Extracted birth year: %d\n", year);
	
	return;
}


