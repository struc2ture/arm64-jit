#pragma once

#include <stdbool.h>
#include <string.h>

#include "common/util.h"
#include "lexer.h"

#define MAX_LOCALS 16

typedef enum
{
    AST_NONE,
    AST_INT,
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_VAR,
    AST_ASSIGN,
    AST_STMT_LIST,
    AST_RETURN,
    AST_EXPR_STMT,
    AST_COUNT

} AstKind;

typedef struct Ast
{
    AstKind kind;
    struct Ast *left;
    struct Ast *right;
    const char *name; // for AST_VAR and AST_ASSIGN
    int value; // for AST_INT
    struct Ast *next_stmt;

} Ast;

typedef struct
{
    const char *name;
    int reg;

} Symbol;

list_define_type(SymbolList, Symbol);

typedef struct Parser
{
    Lexer *lex;
    Token current;

    SymbolList locals;

} Parser;

void parser_init(Parser *p, Lexer *lex);
void parser_advance(Parser *p);
bool parser_accept(Parser *p, TokenKind kind);

// ========================================

Ast *make_node(AstKind kind, Ast *left, Ast *right);
Ast *make_int(int value);
Ast *make_var(const char *name);
Ast *make_assign(const char *name, Ast *rhs);
Ast *make_statement_list(Ast *head);
Ast *make_return(Ast *val);
Ast *make_expr_stmt(Ast *expr);

// ========================================

Ast *parse_factor(Parser *p);
Ast *parse_term(Parser *p);
Ast *parse_expr(Parser *p);
Ast *parse_stmt(Parser *p);
Ast *parse_stmt_list(Parser *p);

// ========================================

void print_ast(Ast *n, int indent);

// ========================================

int find_local(const SymbolList *locals, const char *name);
int expect_local(const SymbolList *locals, const char *name);
int add_local(SymbolList *locals, const char *name);
