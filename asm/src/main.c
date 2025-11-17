#include <stdio.h>

#include "lexer.h"

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

    P8Token t;
    while ((t = LexerNextToken(&inputStream)).type != TT_EOF) {
        printf("Line %d:\tType: %d, length: %zu, lexeme: `%.*s`\n", t.line,
               t.type, t.length, (int)t.length, t.lexeme);
    }

    fclose(fptr);

    return 0;
}
