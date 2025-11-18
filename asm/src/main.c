#include <stdio.h>

#include "parser.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "p8asm: usage: %s <.asm path>\n", argv[0]);
        return 1;
    }

    FILE *fptr;
    fptr = fopen(argv[1], "r");

    if (fptr == NULL) {
        perror("p8asm");
        return 1;
    }

    P8IStream inputStream = {fptr};
    inputStream.line = 1;

    ParserProcess(&inputStream);

    fclose(fptr);

    return 0;
}
