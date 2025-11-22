#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "util.h"

void AddCompileConstant(P8Parser *p, const char *name,
                        P8Token value) {
    if (p->constantCount >= p->constantCapacity) {
        size_t newCapacity = p->constantCapacity * 2;
        P8CompileConstant *newMem =
            realloc(p->constants, newCapacity * sizeof(P8CompileConstant));
        if (!newMem) {
            perror("p8asm: constant realloc");
            exit(1);
        }
        p->constants = newMem;
        p->constantCapacity = newCapacity;
    }
    P8CompileConstant *constant = &p->constants[p->constantCount++];
    snprintf(constant->name, sizeof(constant->name), "%s", name);
    constant->value = value;
}

void ParserProcess(P8Parser *p, P8IStream *in) {
    p->tokenCount = 0;
    p->labelCount = 0;
    p->pc = 0;

    p->constantCapacity = 8;
    p->constantCount = 0;
    p->constants = malloc(p->constantCapacity * sizeof(P8CompileConstant));

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
        } else if (t->type == TT_DEFINEDIR) {
            if (UtilStrCase(t->lexeme, "DEFINE") == 0) {
                ++i;
                P8Token nameTok = p->tokens[i++];
                P8Token valueTok = p->tokens[i];
                AddCompileConstant(p, nameTok.lexeme, valueTok);
            }
        } else if (t->type == TT_OPCODE)
            ++p->pc;
    }

    p->pc = 0;

    for (size_t i = 0; i < p->labelCount; ++i) {
        printf("LABEL `%s` AT %x\n", p->labels[i].name, p->labels[i].address);
    }

    for (size_t i = 0; i < p->constantCount; ++i) {
        printf("CONSTANT `%s` = %s\n", p->constants[i].name,
               p->constants[i].value.lexeme);
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

    P8Token regTok;

    if (p->tokens[p->tokenPtr].type == TT_SYMBOL) {
        // get from constants list
        size_t idx = 0;
        for (size_t i = 0; i < p->constantCount; ++i) {
            if (UtilStrCase(p->constants[i].name,
                            p->tokens[p->tokenPtr].lexeme) == 0) {
                idx = i;
             break;}
        }

        if (p->constants[idx].value.type != TT_REGISTER) {
            fprintf(
                stderr,
                "p8asm: invalid syntax at line %d: expected register, got %d (expanded from .DEFINE)\n",
                    p->tokens[p->tokenPtr].line, p->constants[idx].value.type);
            exit(1);
        }

        regTok = p->constants[idx].value;
        ++p->tokenPtr;
    } else if (p->tokens[p->tokenPtr].type != TT_REGISTER) {
        fprintf(stderr,
                "p8asm: invalid syntax at line %d: expected register, got %d\n",
                p->tokens[p->tokenPtr].line, p->tokens[p->tokenPtr].type);
        exit(1);
    } else 
        regTok = p->tokens[p->tokenPtr++];

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

      for (size_t i = 0; i < p->constantCount; ++i) {
        if (UtilStrCase(p->constants[i].name, immTok.lexeme) == 0) {
            if (p->constants[i].value.type != TT_NUMBER) {
                fprintf(stderr,
                        "p8asm: invalid syntax at line %d: expected "
                        "imm, got %d (expanded from .DEFINE)\n",
                        p->tokens[p->tokenPtr].line,
                        p->constants[i].value.type);
                exit(1);
            }
            return atoi(p->constants[i].value.lexeme);
        }
    }
}

P8Instruction ParserNextInstruction(P8Parser *p) {

DoSkip:
    while ((p->tokens[p->tokenPtr].type == TT_LABELDECL) &&
           p->tokenPtr < p->tokenCount)
        ++p->tokenPtr;

    while ((p->tokens[p->tokenPtr].type == TT_DEFINEDIR) &&
           p->tokenPtr < p->tokenCount)
        p->tokenPtr += 3;

    if ((p->tokens[p->tokenPtr].type == TT_LABELDECL) &&
        p->tokenPtr < p->tokenCount)
        goto DoSkip;

    if (p->tokenPtr >= p->tokenCount) return (P8Instruction){false};

    P8Opcode opcode = ParserExpectOpcode(p);

    switch (opcode) {
        case OP_NOP:
        case OP_HLT: return (P8Instruction){true, opcode};

        case OP_LDI: {
            P8Register reg1 = ParserExpectRegister(p);
            ParserExpectComma(p);
            uint8_t immptr = ParserExpectImm(p);
            return (P8Instruction){true, opcode, reg1, 0, immptr, true};
        }

        case OP_LD: {
            P8Register reg1 = ParserExpectRegister(p);
            ParserExpectComma(p);
            if (p->tokens[p->tokenPtr].type == TT_REGISTER) {
                P8Register reg2 = ParserExpectRegister(p);
                return (P8Instruction){true, opcode, reg1, reg2, 0, true};
            } else {
                uint8_t immptr = ParserExpectImm(p);
                return (P8Instruction){true, opcode, reg1, 0, immptr, false};
            }
        }

        case OP_MOV:
        case OP_ST:
        {
            P8Register reg1 = ParserExpectRegister(p);
            ParserExpectComma(p);
            P8Register reg2 = ParserExpectRegister(p);

            return (P8Instruction){true, opcode, reg1, reg2, 0, true};
        }

        case OP_STI: {
            P8Register reg1 = ParserExpectRegister(p);
            ParserExpectComma(p);
            uint8_t immptr = ParserExpectImm(p);
            return (P8Instruction){true, opcode, reg1, 0, immptr, true};
        }
         
        case OP_JMP:
        case OP_JZ:
        case OP_JC:
        case OP_JO:
        case OP_JN:
        case OP_JV: {
            if (p->tokens[p->tokenPtr].type == TT_REGISTER) {
                P8Register reg1 = ParserExpectRegister(p);
                return (P8Instruction){true, opcode, reg1, 0, 0, true};
            } else {
                uint8_t immptr = ParserExpectImm(p);
                return (P8Instruction){true, opcode, 0, 0, immptr, false};
            }
        }

        case OP_IN:
        case OP_OUT: {
            P8Register reg1 = ParserExpectRegister(p);
            ParserExpectComma(p);
            if (p->tokens[p->tokenPtr].type == TT_REGISTER) {
                P8Register reg2 = ParserExpectRegister(p);
                return (P8Instruction){true, opcode, reg1, reg2, 0, true};
            } else {
                uint8_t immptr = ParserExpectImm(p);
                return (P8Instruction){true, opcode, reg1, 0, immptr, false};
            }
        }

        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_AND:
        case OP_OR:
        case OP_XOR:
        case OP_SHL:
        case OP_SHR:
        case OP_TEST:
        case OP_CMP: {
            P8Register reg1 = ParserExpectRegister(p);
            ParserExpectComma(p);
            if (p->tokens[p->tokenPtr].type == TT_REGISTER) {
                P8Register reg2 = ParserExpectRegister(p);
                return (P8Instruction){true, opcode, reg1, reg2, 0, true};
            } else {
                uint8_t immptr = ParserExpectImm(p);
                return (P8Instruction){true, opcode, reg1, 0, immptr, false};
            }
        }

        case OP_INC:
        case OP_DEC: {
            P8Register reg1 = ParserExpectRegister(p);
            return (P8Instruction){true, opcode, reg1, 0, 0, false};
        }

        default: 
            fprintf(stderr, "p8asm: unhandled opcode\n");
            exit(1);
    }
}