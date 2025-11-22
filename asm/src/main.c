#include <stdio.h>
#include <stdlib.h>

#include "codegen.h"

int main(int argc, char *argv[]) {
    /*if (argc != 3) {
        fprintf(stderr,
                "p8asm: usage: %s <source path> <Logisim hex output path>\n",
                argv[0]);
        return 1;
    }*/

    FILE *fptr;
    //fptr = fopen(argv[1], "r");
     fptr = fopen("../../../fixtures/precompiler.asm", "r");

    if (fptr == NULL) {
        perror("p8asm: source file");
        return 1;
    }

    P8IStream inputStream = {fptr};
    inputStream.line = 1;

    P8Parser parser;
    ParserProcess(&parser, &inputStream);

    fclose(fptr);

    //CodegenHex(&parser, argv[2]);
     CodegenHex(&parser, "../../../fixtures/bin/precompiler.hex");

    free(parser.tokens);
    free(parser.labels);

    return 0;
}
