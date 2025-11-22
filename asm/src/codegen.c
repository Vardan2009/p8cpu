#include "codegen.h"

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

uint32_t EncodeInstruction(P8Instruction i) {
    return ((uint32_t)((uint8_t)i.opcode & 0x1F) << 15) |
           ((uint32_t)((uint8_t)i.reg1 & 0x7) << 12) |
           ((uint32_t)((uint8_t)i.reg2 & 0x7) << 9) |
           ((uint32_t)(i.immptr & 0xFF) << 1) | ((uint32_t)(i.mode & 0x1));
}

static FILE* fptr;

void CodegenOnInstructionHex(P8Instruction inst) {
    printf("OP[%d] R%d R%d %d MODE %d\n", inst.opcode, inst.reg1, inst.reg2,
           inst.immptr, inst.mode);
    fprintf(fptr, "%05X\n", EncodeInstruction(inst) & 0xFFFFF);
}

void CodegenHex(P8Parser* p, const char* path) {
    fptr = fopen(path, "w");

    if (fptr == NULL) {
        perror("p8asm: output file");
        exit(1);
    }

    fprintf(fptr, "v2.0 raw\n");

    ParserRun(p, CodegenOnInstructionHex);

    fclose(fptr);
}