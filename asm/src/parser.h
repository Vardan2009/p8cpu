#ifndef P8_PARSER_H
#define P8_PARSER_H

#include <stdint.h>

#include "lexer.h"

typedef enum {
    OP_NOP,
    OP_LDI,
    OP_LD,
    OP_MOV,
    OP_STI,
    OP_ST,
    OP_JMP,
    OP_JZ,
    OP_JC,
    OP_JO,
    OP_JN,
    OP_JV,
    OP_IN,
    OP_OUT,
    OP_RESV1,
    OP_RESV2,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NOT,
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_SHL,
    OP_SHR,
    OP_INC,
    OP_DEC,
    OP_TEST,
    OP_CMP,
    OP_RESV3,
    OP_HLT,

    // these operations are not directly compiled into machine code, they act
    // kinda like macros
    OP_PUSH,
    OP_POP
} P8Opcode;

typedef enum { R_R0, R_R1, R_R2, R_R3, R_R4, R_R5, R_SP, R_FLAGS } P8Register;

typedef struct {
    bool valid;

    P8Opcode opcode;
    P8Register reg1;
    P8Register reg2;
    uint8_t immptr;
    bool mode;
} P8Instruction;

typedef struct {
    char name[LEXER_LEXEME_SIZE];
    uint8_t address;
} P8Label;

typedef struct {
    char name[LEXER_LEXEME_SIZE];
    P8Token value;
} P8CompileConstant;

typedef struct {
    P8Token *tokens;
    size_t tokenCount;
    size_t tokenCapacity;
    size_t tokenPtr;

    P8Label *labels;
    size_t labelCount;
    size_t labelCapacity;

    P8CompileConstant *constants;
    size_t constantCount;
    size_t constantCapacity;

    uint8_t pc;
} P8Parser;

typedef void (*InstructionCallback)(P8Instruction instr);

void ParserProcess(P8Parser *p, P8IStream *in);
void ParserRun(P8Parser *p, InstructionCallback cb);

#endif  // P8_PARSER_H