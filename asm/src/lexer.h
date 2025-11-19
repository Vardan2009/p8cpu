#ifndef P8_LEXER_H
#define P8_LEXER_H

#include <stdbool.h>
#include <stdio.h>

#define LEXER_BUF_SIZE 1024
#define LEXER_LEXEME_SIZE 64

typedef struct {
    FILE *fp;
    char buf[LEXER_BUF_SIZE];
    size_t len;
    size_t pos, absPos;
    bool eof;

    int line;
} P8IStream;

typedef enum {
    TT_EOF,        // end of file
    TT_NUMBER,     // integer
    TT_SYMBOL,     // any identifier
    TT_OPCODE,     // opcode
    TT_LABELDECL,  // label declaration
    TT_REGISTER,   // register
    TT_COMMA,      // comma
    TT_CURRENTPC   // $
} P8TokenType;

typedef struct {
    P8TokenType type;
    char lexeme[LEXER_LEXEME_SIZE];
    size_t length;
    int line;
} P8Token;

P8Token LexerNextToken(P8IStream *in);

#endif  // P8_LEXER_H
