#include "defs.h"

const char *gInstTokens[NUM_INST_TOKENS] = {
    "HLT", "LIM", "LOD", "SIM", "MOV", "JMP", "JZE", "JNZ",
    "ADD", "SUB", "MUL", "DIV", "NOT", "AND", "OR",  "XOR"};

const char *gRegTokens[NUM_REG_TOKENS] = {"R0", "R1", "R2", "R3",
                                          "R4", "R5", "R6", "R7"};