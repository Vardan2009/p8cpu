#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char LexerNext(P8IStream *in) {
    if (in->pos >= in->len) {
        if (in->eof) return 0;

        in->len = fread(in->buf, 1, LEXER_BUF_SIZE, in->fp);
        in->pos = 0;

        if (in->len == 0) {
            in->eof = true;
            return 0;
        }
    }

    return in->buf[in->pos++];
}

char LexerPeek(P8IStream *in) {
    char c = LexerNext(in);
    if (!c) return 0;
    --in->pos;
    return c;
}

void LexerUnread(P8IStream *in) {
    if (in->pos > 0) --in->pos;
}

static const char *gInstTokens[] = {"HLT", "MOV", "JMP", "JZE", "JNZ",
                                    "ADD", "SUB", "MUL", "DIV", "NOT",
                                    "AND", "OR",  "XOR"};

P8Token LexerNextToken(P8IStream *in) {
    char c;

    while ((c = LexerNext(in))) {
        if (c == '\n') ++in->line;
        if (!isspace(c)) break;
    }

    while (c == ';') {
        while ((c = LexerNext(in))) {
            if (c == '\n') {
                ++in->line;
                c = LexerNext(in);
                break;
            }
        }
    }

    if (!c) return (P8Token){TT_EOF};

    if (isalpha(c) || c == '_') {
        size_t start = in->pos - 1;
        int line = in->line;
        while (true) {
            char p = LexerPeek(in);
            if (isalnum(p) || p == '_')
                LexerNext(in);
            else
                break;
        }

        bool isLabel = false;

        if (LexerPeek(in) == ':') isLabel = true;

        P8Token result = {isLabel ? TT_LABELDECL : TT_SYMBOL};
        result.length = in->pos - start;
        result.line = line;
        strncpy(result.lexeme, &in->buf[start], result.length);

        if (isLabel) LexerNext(in);

        return result;
    } else if (isdigit(c)) {
        size_t start = in->pos - 1;
        int line = in->line;
        while (isdigit(LexerPeek(in))) LexerNext(in);

        P8Token result = {TT_NUMBER};
        result.length = in->pos - start;
        result.line = line;
        strncpy(result.lexeme, &in->buf[start], result.length);
        return result;
    } else if (c == '%') {
        size_t start = in->pos;
        int line = in->line;
        while (true) {
            char p = LexerPeek(in);
            if (isalnum(p))
                LexerNext(in);
            else
                break;
        }

        P8Token result = {TT_REGISTER};
        result.length = in->pos - start;
        result.line = line;
        strncpy(result.lexeme, &in->buf[start], result.length);

        return result;
    }

    switch (c) {
        case '$': return (P8Token){TT_CURRENTPC, "$", 1, in->line};
        case ',': return (P8Token){TT_COMMA, ",", 1, in->line};
    }

    fprintf(stderr, "p8asm: invalid character `%c`\n", c);
    exit(1);
}
