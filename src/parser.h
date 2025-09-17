#pragma once

#include <stdbool.h>

#include "lexer.h"

typedef enum
{
    AST_NONE,
    AST_INT,
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_COUNT

} AstKind;

typedef struct Ast
{
    AstKind kind;
    struct Ast *left;
    struct Ast *right;
    int value;

} Ast;

typedef struct Parser
{
    Lexer *lex;
    Token current;

} Parser;

void parser_init(Parser *p, Lexer *lex);
void parser_advance(Parser *p);
bool parser_accept(Parser *p, TokenKind kind);
Ast *make_node(AstKind kind, Ast *left, Ast *right);
Ast *make_int(int value);
Ast *parse_factor(Parser *p);
Ast *parse_term(Parser *p);
Ast *parse_expr(Parser *p);
void print_ast(Ast *n, int indent);
