#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "defs.h"

void ParserProcess(P8Parser *p, P8IStream *in) {
    p->tokenCount = 0;
    p->labelCount = 0;
    p->pc = 0;

    p->tokenPtr = 0;

    p->tokenCapacity = 16;
    p->tokenCount = 0;
    p->tokens = malloc(p->tokenCapacity * sizeof(P8Token));
    if (!p->tokens) {
        perror("p8asm: token malloc");
        exit(1);
    }

    while (true) {
        if (p->tokenCount >= p->tokenCapacity) {
            size_t newCapacity = p->tokenCapacity * 2;
            P8Token *newMem = realloc(p->tokens, newCapacity * sizeof(P8Token));

            if (!newMem) {
                perror("p8asm: token realloc");
                exit(1);
            }

            p->tokens = newMem;
            p->tokenCapacity = newCapacity;
        }

        P8Token tok = LexerNextToken(in);
        if (tok.type == TT_EOF) break;
        p->tokens[p->tokenCount++] = tok;
    }

    p->labelCapacity = 8;
    p->labelCount = 0;
    p->labels = malloc(p->labelCapacity * sizeof(P8Label));
    if (!p->labels) {
        perror("p8asm: label malloc");
        exit(1);
    }

    for (size_t i = 0; i < p->tokenCount; ++i) {
        P8Token *t = &p->tokens[i];

        if (t->type == TT_LABELDECL) {
            if (p->labelCount >= p->labelCapacity) {
                size_t newCapacity = p->labelCapacity * 2;
                P8Label *newMem =
                    realloc(p->labels, newCapacity * sizeof(P8Label));

                if (!newMem) {
                    perror("p8asm: label realloc");
                    exit(1);
                }

                p->labels = newMem;
                p->labelCapacity = newCapacity;
            }

            P8Label *lab = &p->labels[p->labelCount++];

            snprintf(lab->name, sizeof(lab->name), "%.*s", (int)t->length,
                     t->lexeme);

            lab->address = p->pc;
        } else if (t->type == TT_OPCODE)
            ++p->pc;
    }

    p->pc = 0;

    for (size_t i = 0; i < p->labelCount; ++i) {
        printf("LABEL `%s` AT %x\n", p->labels[i].name, p->labels[i].address);
    }
}

P8Opcode ParserExpectOpcode(P8Parser *p) {
    if (p->tokenPtr >= p->tokenCount) {
        fprintf(stderr,
                "p8asm: invalid syntax at line %d: expected instruction, got "
                "end of "
                "file\n",
                p->tokens[p->tokenPtr].line);
        exit(1);
    }

    if (p->tokens[p->tokenPtr].type != TT_OPCODE) {
        fprintf(
            stderr,
            "p8asm: invalid syntax at line %d: expected opcode, got type %d\n",
            p->tokens[p->tokenPtr].line, p->tokens[p->tokenPtr].type);
        exit(1);
    }

    P8Token opcodeTok = p->tokens[p->tokenPtr++];

    for (size_t i = 0; i < NUM_INST_TOKENS; ++i)
        if (UtilStrCase(opcodeTok.lexeme, gInstTokens[i]) == 0) return i;
}

void ParserExpectComma(P8Parser *p) {
    if (p->tokenPtr >= p->tokenCount) {
        fprintf(stderr,
                "p8asm: invalid syntax at line %d: expected comma, got end of "
                "file\n",
                p->tokens[p->tokenPtr].line);
        exit(1);
    }

    if (p->tokens[p->tokenPtr].type != TT_COMMA) {
        fprintf(
            stderr,
            "p8asm: invalid syntax at line %d: expected comma, got type %d\n",
            p->tokens[p->tokenPtr].line, p->tokens[p->tokenPtr].type);
        exit(1);
    }

    ++p->tokenPtr;
}

P8Register ParserExpectRegister(P8Parser *p) {
    if (p->tokenPtr >= p->tokenCount) {
        fprintf(
            stderr,
            "p8asm: invalid syntax at line %d: expected register, got end of "
            "file\n",
            p->tokens[p->tokenPtr].line);
        exit(1);
    }

    if (p->tokens[p->tokenPtr].type != TT_REGISTER) {
        fprintf(stderr,
                "p8asm: invalid syntax at line %d: expected register, got %d\n",
                p->tokens[p->tokenPtr].line, p->tokens[p->tokenPtr].type);
        exit(1);
    }

    P8Token regTok = p->tokens[p->tokenPtr++];

    for (size_t i = 0; i < NUM_INST_TOKENS; ++i)
        if (UtilStrCase(regTok.lexeme, gRegTokens[i]) == 0) return i;
}

uint8_t ParserExpectImm(P8Parser *p) {
    if (p->tokenPtr >= p->tokenCount) {
        fprintf(stderr,
                "p8asm: invalid syntax at line %d: expected number/symbol, got "
                "end of "
                "file\n",
                p->tokens[p->tokenPtr].line);
        exit(1);
    }

    if (p->tokens[p->tokenPtr].type != TT_SYMBOL &&
        p->tokens[p->tokenPtr].type != TT_NUMBER &&
        p->tokens[p->tokenPtr].type != TT_CURRENTPC) {
        fprintf(stderr,
                "p8asm: invalid syntax at line %d: expected number/symbol, got "
                "%d\n",
                p->tokens[p->tokenPtr].line, p->tokens[p->tokenPtr].type);
        exit(1);
    }

    P8Token immTok = p->tokens[p->tokenPtr++];

    if (immTok.type == TT_NUMBER) return atoi(immTok.lexeme);
    if (immTok.type == TT_CURRENTPC) return p->pc;

    for (size_t i = 0; i < p->labelCount; ++i)
        if (UtilStrCase(p->labels[i].name, immTok.lexeme) == 0)
            return p->labels[i].address;
}

P8Instruction ParserNextInstruction(P8Parser *p) {
    while (p->tokens[p->tokenPtr].type == TT_LABELDECL &&
           p->tokenPtr < p->tokenCount)
        ++p->tokenPtr;

    if (p->tokenPtr >= p->tokenCount) return (P8Instruction){false};

    P8Opcode opcode = ParserExpectOpcode(p);

    switch (opcode) {
        case OP_HLT: return (P8Instruction){true, opcode};

        case OP_LIM:
        case OP_LOD: {
            P8Register reg1 = ParserExpectRegister(p);
            ParserExpectComma(p);
            uint8_t imm = ParserExpectImm(p);
            ++p->pc;
            return (P8Instruction){true, opcode, reg1, 0, imm};
        }

        case OP_JMP: {
            uint8_t imm = ParserExpectImm(p);
            ++p->pc;
            return (P8Instruction){true, opcode, 0, 0, imm};
        }

        case OP_JZE:
        case OP_JNZ: {
            P8Register reg1 = ParserExpectRegister(p);
            ParserExpectComma(p);
            uint8_t imm = ParserExpectImm(p);
            ++p->pc;
            return (P8Instruction){true, opcode, reg1, 0, imm};
        }

        case OP_MOV:
        case OP_STR:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_NOT:
        case OP_AND:
        case OP_OR:
        case OP_XOR: {
            P8Register reg1 = ParserExpectRegister(p);
            ParserExpectComma(p);
            P8Register reg2 = ParserExpectRegister(p);
            ++p->pc;
            return (P8Instruction){true, opcode, reg1, reg2};
        }
    }
}