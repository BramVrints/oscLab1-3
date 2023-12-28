#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]){
    int result =1;
    char userPass[12];
    char* password = "Passwrd123!";
    strcpy(userPass, argv[1]);
    result = strcmp(userPass, password);
    char msg[32];

    gets(msg);
    if(result ==0){
        printf("Password correct!\n");
        printf("Printing Message %s\n", msg);
    } else {
        printf("Wrong!\n");
    }
    return 0;
}