#include <stdio.h>
#include <common/arena.h>
#include <frontend/lexer.h>
#include <frontend/parser.h>


int main(int argc, char *argv[])
{
    const char* source;
    if (argc == 0)
    {
        printf("Enter 1 number\n");
        return 1;
    }
    if (argc > 2)
    {
        printf("Enter ONLY 1 number\n");
        return 1;
    } else {
        source = argv[1];
    }

    Arena arena = {0};
    arena_init(&arena);

    Lexer lexer = {0};
    lexer_init(&lexer, source);

    Parser parser = {0};
    parser_init(&parser, &lexer);


    ASTNode* node = parse_primary(&arena, &parser);
    printf("Node type: %s\n", ASTTypeStr(node->type));

    return 0;
}
