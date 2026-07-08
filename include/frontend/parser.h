#pragma once

#include "lexer.h"
#include "common/arena.h"
#include <stdlib.h>



typedef struct {
    Lexer* lexer;
    Token current;
} Parser;


typedef struct ASTBlockStmt {
    ASTNode* node;
    struct ASTBlockStmt* next;
} ASTBlockStmt;


typedef enum {
    AST_PROGRAM,

    //Statment
    AST_STMT_IF,
    AST_STMT_BLOCK,
    AST_STMT_RETURN,
    AST_STMT_VARDECL,
    AST_STMT_ASSIGNMENT,
    AST_STMT_EXPR,

    //Expression
    AST_EXPR_BINARY,
    AST_EXPR_UNARY,
    AST_EXPR_CALL,
    AST_EXPR_NUMBER,
    AST_EXPR_IDENT
} ASTType;


typedef struct ASTNode {
    ASTType type;
} ASTNode;


typedef struct {
    ASTNode base;

    ASTNode** statements;
    int count;
} ASTProgram;


typedef struct {
    ASTNode base;
    Token token;
} ASTNumberExpr;


typedef struct {
    ASTNode base;
    Token token;
} ASTIdentExpr;


typedef struct {
    ASTNode base;

    ASTNode* lhs;
    ASTNode* rhs;

    Token op;
} ASTBinaryExpr;


typedef struct {
    ASTNode base;

    Token name_token;
    Token type_token;

    ASTNode* initializer;
} ASTVarDeclStmt;


typedef struct {
    ASTNode base;

    Token name_token;
    ASTNode* value;
} ASTAssignmentStmt;


typedef struct {
    ASTNode base;
    ASTBlockStmt* first;
} ASTBlock;



const char *ASTTypeStr(ASTType type);
void parser_init(Parser* p, Lexer* l);
ASTNode* parse_statement(Arena* a, Parser* p);
ASTNode* parse_block(Arena* a, Parser* p);
