#include "parser.h"

#include <stdbool.h>

#include "common/util.h"
#include "lexer.h"

void parser_init(Parser *p, Lexer *lex)
{
    p->lex = lex;
    parser_advance(p);
}

void parser_advance(Parser *p)
{
    p->current = lexer_next(p->lex);
}

bool parser_accept(Parser *p, TokenKind kind)
{
    if (p->current.kind == kind)
    {
        parser_advance(p);
        return true;
    }
    return false;
}

Ast *make_node(AstKind kind, Ast *left, Ast *right)
{
    Ast *n = xmalloc(sizeof(*n));
    *n = (Ast){
        .kind = kind,
        .left = left,
        .right = right
    };
    return n;
}

Ast *make_int(int value)
{
    Ast *n = xmalloc(sizeof(*n));
    *n = (Ast){
        .kind = AST_INT,
        .value = value
    };
    return n;
}

Ast *parse_factor(Parser *p)
{
    if (p->current.kind == TOK_INT)
    {
        int val = p->current.value;
        parser_advance(p);
        return make_int(val);
    }
    else if (parser_accept(p, TOK_LPAREN))
    {
        Ast *e = parse_expr(p);
        if (!parser_accept(p, TOK_RPAREN))
        {
            fatal("Expected ')'");
        }
        return e;
    }
    else
    {
        fatal("Unexpected token in factor: %s", tok_kind_str(p->current.kind));
    }
}

Ast *parse_term(Parser *p)
{
    Ast *node = parse_factor(p);
    while (p->current.kind == TOK_STAR || p->current.kind == TOK_SLASH)
    {
        TokenKind op = p->current.kind;
        parser_advance(p);
        Ast *right = parse_factor(p);
        node = make_node(op == TOK_STAR ? AST_MUL : AST_DIV, node, right);
    }
    return node;
}

Ast *parse_expr(Parser *p)
{
    Ast *node = parse_term(p);
    while (p->current.kind == TOK_PLUS || p->current.kind == TOK_MINUS)
    {
        TokenKind op = p->current.kind;
        parser_advance(p);
        Ast *right = parse_term(p);
        node = make_node(op == TOK_PLUS ? AST_ADD : AST_SUB, node, right);
    }
    return node;
}

void print_ast(Ast *n, int indent)
{
    if (!n || n->kind == AST_NONE) return;
    for (int i = 0; i < indent * 2; i++) printf(" ");
    switch (n->kind)
    {
        case AST_INT: printf("INT: %d\n", n->value); break;
        case AST_ADD: printf("ADD\n"); break;
        case AST_SUB: printf("SUB\n"); break;
        case AST_MUL: printf("MUL\n"); break;
        case AST_DIV: printf("DIV\n"); break;
        case AST_NONE: case AST_COUNT: printf("UNKNOWN\n");
    }
    print_ast(n->left, indent + 1);
    print_ast(n->right, indent + 1);
}
