#include "common/types.h"
#include "common/util.h"

#include "codegen.h"
#include "lexer.h"
#include "parser.h"

int main()
{
    log("Lexer test");
    {
        const char *input = "1 + 2 * (3 - 4)";

        Lexer lex;
        lexer_init(&lex, input);

        Token tok;
        do
        {
            tok = lexer_next(&lex);
            printf("Token: %s", tok_kind_str(tok.kind));
            if (tok.kind == TOK_INT)
            {
                printf(" (value = %d)", tok.value);
            }
            printf("\n");
        }
        while (tok.kind != TOK_EOF && tok.kind != TOK_NONE);
    }
    log("");

    log("Parser test");
    {
        const char *input = "1 + 2 * (3 - 4)";

        Lexer lex;
        lexer_init(&lex, input);

        Parser parser;
        parser_init(&parser, &lex);

        Ast *root = parse_expr(&parser);
        print_ast(root, 0);
    }
    log("");

    log("Codegen test");
    {
        const char *input = "1 + 2 * (3 - 4)";

        Lexer lex;
        lexer_init(&lex, input);

        Parser parser;
        parser_init(&parser, &lex);
        Ast *root = parse_expr(&parser);
        print_ast(root, 0);

        CodeBuffer cb = cb_create(4096);
        codegen_expr(&cb, root, 0);
        emit_ret(&cb);

        expr_fn_t fn = (expr_fn_t)cb_get_proc(&cb);
        i64 result = (i64)fn();

        log("Result %lld", result); 
    }
    log("");

    log("Codegen test 2");
    {
        const char *input = "2 * 3 * 4 * 5";

        Lexer lex;
        lexer_init(&lex, input);

        Parser parser;
        parser_init(&parser, &lex);
        Ast *root = parse_expr(&parser);
        print_ast(root, 0);

        CodeBuffer cb = cb_create(4096);
        codegen_expr(&cb, root, 0);
        emit_ret(&cb);

        expr_fn_t fn = (expr_fn_t)cb_get_proc(&cb);
        i64 result = (i64)fn();

        log("Result %lld", result); 
    }
    log("");

    log("Codegen test 3");
    {
        const char *input = "1 + 2 + 3 - 4";

        Lexer lex;
        lexer_init(&lex, input);

        Parser parser;
        parser_init(&parser, &lex);
        Ast *root = parse_expr(&parser);
        print_ast(root, 0);

        CodeBuffer cb = cb_create(4096);
        codegen_expr(&cb, root, 0);
        emit_ret(&cb);

        expr_fn_t fn = (expr_fn_t)cb_get_proc(&cb);
        i64 result = (i64)fn();

        log("Result %lld", result); 
    }
    log("");

    log("Codegen test 3");
    {
        const char *input = "1 + 2 + 3 - 4";

        Lexer lex;
        lexer_init(&lex, input);

        Parser parser;
        parser_init(&parser, &lex);
        Ast *root = parse_expr(&parser);
        print_ast(root, 0);

        CodeBuffer cb = cb_create(4096);
        codegen_expr(&cb, root, 0);
        emit_ret(&cb);

        expr_fn_t fn = (expr_fn_t)cb_get_proc(&cb);
        i64 result = (i64)fn();

        log("Result %lld", result); 
    }
    log("");

    return 0;
}
