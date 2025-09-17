#include "lexer.h"

// ================================================

const char *tok_kind_str(TokenKind kind)
{
    switch (kind)
    {
        case TOK_NONE: return "NONE";
        case TOK_INT: return "Int";
        case TOK_PLUS: return "Plus";
        case TOK_MINUS: return "Minus";
        case TOK_STAR: return "Star";
        case TOK_SLASH: return "Slash";
        case TOK_LPAREN: return "LParen";
        case TOK_RPAREN: return "RParen";
        case TOK_EOF: return "EOF";
        default: return "UNKNOWN";
    }
}

// ================================================

void lexer_init(Lexer *lex, const char *src)
{
    lex->src = src;
    lex->pos = 0;
    lexer_advance(lex);
}

void lexer_advance(Lexer *lex)
{
    lex->ch = lex->src[lex->pos];
    if (lex->ch != '\0')
    {
        lex->pos++;
    }
}

int lex_number(Lexer *lex)
{
    int value = 0;
    while (lex->ch >= '0' && lex->ch <= '9')
    {
        value = value * 10 + (lex->ch - '0');
        lexer_advance(lex);
    }
    return value;
}

Token lexer_next(Lexer *lex)
{
    while (lex->ch == ' ' || lex->ch == '\t' || lex->ch == '\n')
    {
        lexer_advance(lex);
    }

    if (lex->ch >= '0' && lex->ch <= '9')
    {
        int val = lex_number(lex);
        return (Token){
            .kind = TOK_INT,
            .value = val
        };
    }

    Token tok = {0};
    switch (lex->ch)
    {
        case '+': tok.kind = TOK_PLUS; break;
        case '-': tok.kind = TOK_MINUS; break;
        case '*': tok.kind = TOK_STAR; break;
        case '/': tok.kind = TOK_SLASH; break;
        case '(': tok.kind = TOK_LPAREN; break;
        case ')': tok.kind = TOK_RPAREN; break;
        case '\0': tok.kind = TOK_EOF; break;
        default: tok.kind = TOK_NONE; break;
    }

    lexer_advance(lex);
    return tok;
}
