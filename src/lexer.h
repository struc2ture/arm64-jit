#pragma once

#include <stdlib.h>

typedef enum
{
    TOK_NONE,
    TOK_INT,
    TOK_IDENT,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_EQ,
    TOK_SEMI,
    TOK_EOF,
    TOK_COUNT

} TokenKind;

typedef struct Token
{
    TokenKind kind;
    int value;
    const char *text; // for TOK_IDENT

} Token;

typedef struct Lexer
{
    const char *src;
    size_t pos;
    char ch;

} Lexer;

const char *tok_kind_str(TokenKind kind);

void lexer_init(Lexer *lex, const char *src);
void lexer_advance(Lexer *lex);
Token lexer_next(Lexer *lex);
