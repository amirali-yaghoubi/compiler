#include <stdio.h>
#include <common/arena.h>
#include <frontend/lexer.h>
#include <frontend/parser.h>



#include <stdio.h>

static void print_ast(ASTNode* node)
{
    if (!node) return;

    switch (node->type)
    {
        case AST_EXPR_NUMBER:
        {
            ASTNumberExpr* n = (ASTNumberExpr*)node;
            printf("%ld", n->token.value.int_value);
            break;
        }

        case AST_EXPR_IDENT:
        {
            ASTIdentExpr* id = (ASTIdentExpr*)node;
            printf("%s", TokenTypeStr(id->token.type));
            break;
        }

        case AST_EXPR_BINARY:
        {
            ASTBinaryExpr* b = (ASTBinaryExpr*)node;

            printf("(");
            print_ast(b->lhs);

            // چاپ operator
            switch (b->op.type)
            {
                case TOK_PLUS:      printf(" + "); break;
                case TOK_MINUS:     printf(" - "); break;
                case TOK_MULTIPLY:  printf(" * "); break;
                case TOK_DIVIDE:    printf(" / "); break;
                default:            printf(" ? "); break;
            }

            print_ast(b->rhs);
            printf(")");
            break;
        }

        default:
            printf("<unknown>");
            break;
    }
}





int main()
{
    const char* source = "((2 + 3) * (4 + 5))";

    Arena arena = {0};
    arena_init(&arena);

    Lexer lexer = {0};
    lexer_init(&lexer, source);

    Parser parser = {0};
    parser_init(&parser, &lexer);


    ASTNode* root = parse_expression(&arena, &parser);

    print_ast(root);
    printf("\n");


    return 0;
}
