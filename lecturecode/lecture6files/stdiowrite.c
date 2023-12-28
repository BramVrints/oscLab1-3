//
// Created by osc on 28/10/23.
//

#include <stdio.h>

main() {
    FILE *fp;

    fp = fopen("test.txt", "w+");
    fprintf(fp, "This is testing for %s...\n", "fprintf");
    fputs("This is testing for fputs...\n", fp);
    fputc('3', fp);
    fclose(fp);
}
