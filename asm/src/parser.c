#include "parser.h"

#include <stdlib.h>
#include <string.h>

void ParserProcess(P8Parser *p, P8IStream *in) {
    p->tokenCount = 0;
    p->labelCount = 0;
    p->pc = 0;

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

    for (size_t i = 0; i < p->labelCount; ++i) {
        printf("LABEL `%s` AT %x\n", p->labels[i].name, p->labels[i].address);
    }
}

P8Instruction ParserNextInstruction() {}