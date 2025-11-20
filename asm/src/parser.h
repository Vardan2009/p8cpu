#ifndef P8_PARSER_H
#define P8_PARSER_H

#include <stdint.h>

#include "lexer.h"

typedef enum {
    OP_HLT,
    OP_LIM,
    OP_LOD,
    OP_STR,
    OP_MOV,
    OP_JMP,
    OP_JZE,
    OP_JNZ,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NOT,
    OP_AND,
    OP_OR,
    OP_XOR
} P8Opcode;

typedef enum {
    R_R0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_SP
} P8Register;

typedef struct {
    bool valid;

    P8Opcode opcode;
    P8Register reg1;
    P8Register reg2;
    uint8_t immptr;
} P8Instruction;

typedef struct {
    char name[LEXER_LEXEME_SIZE];
    uint8_t address;
} P8Label;

typedef struct {
    P8Token *tokens;
    size_t tokenCount;
    size_t tokenCapacity;
    size_t tokenPtr;

    P8Label *labels;
    size_t labelCount;
    size_t labelCapacity;

    uint8_t pc;
} P8Parser;

void ParserProcess(P8Parser *p, P8IStream *in);
P8Instruction ParserNextInstruction(P8Parser *p);

#endif  // P8_PARSER_H