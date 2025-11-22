#include "defs.h"

const char *gInstTokens[NUM_INST_TOKENS] = {
    "NOP", "LDI",  "LD",  "MOV",   "STI", "ST",    "JMP",   "JZ",  "JC",
    "JO",  "JN",   "JV",  "IN",    "OUT", "RESV1", "RESV2", "ADD", "SUB",
    "MUL", "DIV",  "NOT", "AND",   "OR",  "XOR",   "SHL",   "SHR", "INC",
    "DEC", "TEST", "CMP", "RESV3", "HLT", "PUSH",  "POP"};

const char *gRegTokens[NUM_REG_TOKENS] = {"R0", "R1", "R2", "R3",
                                          "R4", "R5", "SP", "FLAGS"};