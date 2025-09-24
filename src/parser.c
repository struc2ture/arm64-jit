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

void parser_expect(Parser *p, TokenKind kind)
{
    if (p->current.kind != kind)
    {
        fatal("Parser: expected token %s, but got %s",
            tok_kind_str(kind),
            tok_kind_str(p->current.kind));
    }
    parser_advance(p);
}

// ========================================

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

Ast *make_var(const char *name)
{
    Ast *n = xmalloc(sizeof(*n));
    *n = (Ast){
        .kind = AST_VAR,
        .name = name
    };
    return n;
}

Ast *make_assign(const char *name, Ast *rhs)
{
    Ast *n = xmalloc(sizeof(*n));
    *n = (Ast){
        .kind = AST_ASSIGN,
        .name = name,
        .left = rhs
    };
    return n;
}

Ast *make_statement_list(Ast *head)
{
    Ast *n = xmalloc(sizeof(*n));
    *n = (Ast){
        .kind = AST_STMT_LIST,
        .next_stmt = head
    };
    return n;
}

Ast *make_return(Ast *val)
{
    Ast *n = xmalloc(sizeof(*n));
    *n = (Ast){
        .kind = AST_RETURN,
        .left = val
    };
    return n;
}

Ast *make_expr_stmt(Ast *expr)
{
    Ast *n = xmalloc(sizeof(*n));
    *n = (Ast){
        .kind = AST_EXPR_STMT,
        .left = expr
    };
    return n;
}

// ========================================

Ast *parse_factor(Parser *p)
{
    if (p->current.kind == TOK_INT)
    {
        int val = p->current.value;
        parser_advance(p);
        return make_int(val);
    }
    else if (p->current.kind == TOK_IDENT)
    {
        const char *name = strdup(p->current.text);
        parser_advance(p);

        if (parser_accept(p, TOK_EQ))
        {
            Ast *rhs = parse_expr(p);

            int reg = find_local(&p->locals, name);
            if (reg < 0)
            {
                reg = add_local(&p->locals, name);
            }

            return make_assign(name, rhs);
        }

        int reg = expect_local(&p->locals, name);

        return make_var(name);
    }
    else if (parser_accept(p, TOK_LPAREN))
    {
        Ast *e = parse_expr(p);
        if (!parser_accept(p, TOK_RPAREN))
        {
            fatal("Parser: expected ')'");
        }
        return e;
    }
    else
    {
        fatal("Parser: unexpected token in factor: %s", tok_kind_str(p->current.kind));
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

Ast *parse_stmt(Parser *p)
{
    if (parser_accept(p, TOK_RETURN))
    {
        Ast *val = parse_expr(p);
        parser_expect(p, TOK_SEMI);
        return make_return(val);
    }
    else
    {
        Ast *expr = parse_expr(p);
        parser_expect(p, TOK_SEMI);
        return make_expr_stmt(expr);
    }
}

Ast *parse_stmt_list(Parser *p)
{
    Ast *head = NULL;
    Ast **tail = &head;

    while (p->current.kind != TOK_EOF)
    {
        Ast *stmt = parse_stmt(p);
        *tail = stmt;
        tail = &stmt->next_stmt;
    }

    return make_statement_list(head);
}

// ========================================

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
        case AST_VAR: printf("VAR: %s\n", n->name); break;
        case AST_ASSIGN: printf("ASSIGN: %s\n", n->name); break;
        case AST_STMT_LIST: printf("STMT_LIST\n"); break;
        case AST_RETURN: printf("RETURN\n"); break;
        case AST_EXPR_STMT: printf("EXPR_STMT\n"); break;
        case AST_NONE: case AST_COUNT: printf("UNKNOWN\n");
    }

    if (n->kind == AST_STMT_LIST)
    {
        Ast *stmt = n->next_stmt;
        while (stmt)
        {
            print_ast(stmt, indent + 1);
            stmt = stmt->next_stmt;
        }
    }

    print_ast(n->left, indent + 1);
    print_ast(n->right, indent + 1);
}

// ========================================

int find_local(const SymbolList *locals, const char *name)
{
    const Symbol *symbol_it;
    list_iterate(locals, i, symbol_it)
    {
        if (strcmp(symbol_it->name, name) == 0)
        {
            return symbol_it->reg;
        }
    }
    return -1;
}

int expect_local(const SymbolList *locals, const char *name)
{
    int reg = find_local(locals, name);
    if (reg < 0)
    {
        fatal("Parser: variable %s is used before assignment", name);
    }
    return reg;
}

int add_local(SymbolList *locals, const char *name)
{
    if (locals->size >= MAX_LOCALS)
    {
        fatal("Too many locals");
    }
    int reg = 2 + locals->size; // reserve x0 and x1 for temps
    Symbol symbol =
    {
        .name = strdup(name),
        .reg = reg
    };
    list_append(locals, symbol);
    return reg;
}
