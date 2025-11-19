#include <stdio.h>

#include "parser.h"

int main(int argc, char *argv[]) {
    /*if (argc != 2) {
        fprintf(stderr, "p8asm: usage: %s <.asm path>\n", argv[0]);
        return 1;
    }*/

    FILE *fptr;
    // fptr = fopen(argv[1], "r");
    fptr = fopen("../../../fixtures/incloop.asm", "r");

    if (fptr == NULL) {
        perror("p8asm");
        return 1;
    }

    P8IStream inputStream = {fptr};
    inputStream.line = 1;

    P8Parser parser;
    ParserProcess(&parser, &inputStream);

    P8Instruction inst;
    while ((inst = ParserNextInstruction(&parser)).valid) {
        printf("OP[%d] R%d R%d %d\n", inst.opcode, inst.reg1, inst.reg2,
               inst.immptr);
    }

    fclose(fptr);
    free(parser.labels);
    free(parser.tokens);

    return 0;
}
