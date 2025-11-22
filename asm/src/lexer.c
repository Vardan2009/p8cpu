#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "util.h"

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

    ++in->absPos;
    return in->buf[in->pos++];
}

char LexerPeek(P8IStream *in) {
    char c = LexerNext(in);
    if (!c) return 0;
    --in->pos;
    --in->absPos;
    return c;
}

void LexerUnread(P8IStream *in) {
    if (in->pos > 0) {
        --in->pos;
        --in->absPos;
    }
}

P8Token LexerNextToken(P8IStream *in) {
    char c;

DoSkip:
    while (isspace(c = LexerNext(in)))
        if (c == '\n') ++in->line;

    while (c == ';') {
        while ((c = LexerNext(in))) {
            if (c == '\n') {
                ++in->line;
                c = LexerNext(in);
                break;
            }
        }
    }

    if (isspace(c)) goto DoSkip;

    if (!c) return (P8Token){TT_EOF};

    if (c == '\'') {
        int line = in->line;
        char c = LexerNext(in);
        if (LexerNext(in) != '\'') {
            fprintf(stderr, "p8asm: unclosed char \'\n");
            exit(1);
        }

        P8Token result = {TT_NUMBER};

        sprintf(result.lexeme, "%d", c);

        result.length = 1;
        result.line = line;

        return result;
    }

    if (isalpha(c) || c == '_') {
        size_t start = in->absPos - 1;
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
        result.length = in->absPos - start;
        result.line = line;
        strncpy(result.lexeme, &in->buf[start], result.length);

        for (size_t i = 0; i < NUM_INST_TOKENS; ++i) {
            if (UtilStrCase(result.lexeme, gInstTokens[i]) == 0) {
                result.type = TT_OPCODE;
                break;
            }
        }

        if (isLabel) LexerNext(in);

        return result;
    } else if (isdigit(c)) {
        size_t start = in->absPos - 1;
        int line = in->line;
        while (isdigit(LexerPeek(in))) LexerNext(in);

        P8Token result = {TT_NUMBER};
        result.length = in->absPos - start;
        result.line = line;
        strncpy(result.lexeme, &in->buf[start], result.length);
        return result;
    } else if (c == '%' || c == '.') {
        char sc = c;
        size_t start = in->absPos;
        int line = in->line;
        while (true) {
            char p = LexerPeek(in);
            if (isalnum(p))
                LexerNext(in);
            else
                break;
        }

        P8Token result = {sc == '%' ? TT_REGISTER : TT_PRECOMPILER};
        result.length = in->absPos - start;
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
