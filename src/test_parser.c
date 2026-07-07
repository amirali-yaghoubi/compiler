#include <stdio.h>
#include "common/arena.h"
#include "frontend/lexer.h"
#include "frontend/parser.h"
#include "common/ast_json.h"


void indent(int depth);
void ast_dump(ASTNode* node, int depth);

int main()
{
    const char* source = "let x_5 : int = 8 + 9 * 3 - (8 / 8 + 6 * 0);"; 


    Arena arena = {0};
    arena_init(&arena);

    Lexer lexer = {0};
    lexer_init(&lexer, source);

    Parser parser = {0};
    parser_init(&parser, &lexer);


    ASTNode* root = parse_declaration(&arena, &parser);

    ast_dump(root, 0);

    ast_dump_json(root, "/root/proj/myLang/etc/ast.json");


    return 0;
}





void indent(int depth) {
    for (int i = 0; i < depth; i++) printf("  ");
}

void ast_dump(ASTNode* node, int depth) {
    if (!node) { indent(depth); printf("(null)\n"); return; }

    switch (node->type) {
        case AST_EXPR_NUMBER: {
            ASTNumberExpr* n = (ASTNumberExpr*)node;
            indent(depth);
            printf("Number: %.*s\n", n->token.length, n->token.start);
            break;
        }
        case AST_EXPR_IDENT: {
            ASTIdentExpr* id = (ASTIdentExpr*)node;
            indent(depth);
            printf("Ident: %.*s\n", id->token.length, id->token.start);
            break;
        }
        case AST_EXPR_BINARY: {
            ASTBinaryExpr* bin = (ASTBinaryExpr*)node;
            indent(depth);
            printf("Binary: %.*s\n", bin->op.length, bin->op.start);
            ast_dump(bin->lhs, depth + 1);
            ast_dump(bin->rhs, depth + 1);
            break;
        }
        case AST_STMT_VARDECL: {
            ASTVarDeclStmt* decl = (ASTVarDeclStmt*)node;
            indent(depth);
            printf("VarDecl: %.*s : %.*s =\n",
                   decl->name_token.length, decl->name_token.start,
                   decl->type_token.length, decl->type_token.start);
            ast_dump(decl->initializer, depth + 1);
            break;
        }
        default:
            indent(depth);
            printf("%s (no dumper written yet)\n", ASTTypeStr(node->type));
            break;
    }
}
