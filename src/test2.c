#include "common/types.h"
#include "common/util.h"

#include "lexer.h"
#include "parser.h"

int main()
{
    // Lexer test 1
    if (false)
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

    // Parser test 1
    {
        const char *input = "1 + 2 * (3 - 4)";

        Lexer lex;
        lexer_init(&lex, input);

        Parser parser;
        parser_init(&parser, &lex);

        Ast *root = parse_expr(&parser);
        print_ast(root, 0);
    }

    return 0;
}
