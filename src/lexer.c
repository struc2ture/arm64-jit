#include "lexer.h"

#include <string.h>

// ================================================

const char *tok_kind_str(TokenKind kind)
{
    switch (kind)
    {
        case TOK_NONE: return "NONE";
        case TOK_INT: return "Int";
        case TOK_IDENT: return "Ident";
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
    while (lex->ch == ' ' ||
           lex->ch == '\t' ||
           lex->ch == '\n')
    {
        lexer_advance(lex);
    }

    if ((lex->ch >= 'a' && lex->ch <= 'z') ||
        (lex->ch >= 'A' && lex->ch <= 'Z') ||
         lex->ch == '_')
    {
        size_t start = lex->pos - 1;
        while ((lex->ch >= 'a' && lex->ch <= 'z') ||
               (lex->ch >= 'A' && lex->ch <= 'Z') ||
               (lex->ch >= '0' && lex->ch <= '9') ||
                lex->ch == '_')
        {
            lexer_advance(lex);
        }

        size_t len = lex->pos - start - 1;

        if (strncmp(&lex->src[start], "return", len) == 0)
        {
            return (Token){
                .kind = TOK_RETURN
            };
        }
        else
        {
            char *text = strndup(&lex->src[start], len);
            return (Token){
                .kind = TOK_IDENT,
                .text = text
            };
        }
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
        case '=': tok.kind = TOK_EQ; break;
        case ';': tok.kind = TOK_SEMI; break;
        case '\0': tok.kind = TOK_EOF; break;
        default: tok.kind = TOK_NONE; break;
    }

    lexer_advance(lex);
    return tok;
}
