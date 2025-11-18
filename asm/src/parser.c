#include "parser.h"

#include <string.h>

static P8Token gTokensBuffer[1024];
static size_t gTokenCount = 0;

typedef struct {
    char name[LEXER_LEXEME_SIZE];
    uint8_t address;
} P8Label;

static P8Label gLabels[1024];
static size_t gLabelCount = 0;

void ParserProcess(P8IStream *in) {
    while ((gTokensBuffer[gTokenCount++] = LexerNextToken(in)).type != TT_EOF);

    uint8_t pc = 0;
    for (int i = 0; i < gTokenCount; ++i) {
        P8Token *t = &gTokensBuffer[i];

        if (t->type == TT_LABELDECL) {
            size_t idx = gLabelCount++;
            strncpy(gLabels[idx].name, t->lexeme, t->length);
            gLabels[idx].address = pc;
        } else if (t->type == TT_OPCODE)
            ++pc;
    }
}

P8Instruction ParserNextInstruction() {}