#include "frontend/parser.h"
#include "frontend/lexer.h"
#include "common/arena.h"
#include <stdio.h>
#include <stdbool.h>




const char* ASTTypeStr(ASTType type) {
    switch (type) {
        case AST_PROGRAM:        return "AST_PROGRAM";
        case AST_STMT_IF:        return "AST_STMT_IF";
        case AST_STMT_BLOCK:     return "AST_STMT_BLOCK";
        case AST_STMT_RETURN:    return "AST_STMT_RETURN";
        case AST_STMT_VARDECL:   return "AST_STMT_VARDECL";
        case AST_STMT_EXPR:      return "AST_STMT_EXPR";
        case AST_EXPR_BINARY:    return "AST_EXPR_BINARY";
        case AST_EXPR_UNARY:     return "AST_EXPR_UNARY";
        case AST_EXPR_CALL:      return "AST_EXPR_CALL";
        case AST_EXPR_NUMBER:    return "AST_EXPR_NUMBER";
        case AST_EXPR_IDENT:     return "AST_EXPR_IDENT";
        default:                 return "INVALID_AST_TYPE";
    }
}


void parser_init(Parser* p, Lexer* l)
{
    p->lexer = l;
    p->current = get_next_token(l);
}

//====Helpers====
static Token advance(Parser* p)
{
    Token old = p->current;
    p->current = get_next_token(p->lexer);
    return old;
}


static bool check(Parser* p, TokenType type)
{
    return p->current.type == type;
}


static bool match(Parser* p, TokenType type)
{
    if (check(p, type))
    {
        advance(p);
        return true;
    }
    return false;
}


static Token expect(Parser* p, TokenType type)
{ 
    if (check(p, type))
        return advance(p);
    fprintf(stderr, "Syntax error: expected token type %d in line %d\n", type, p->lexer->line);
    exit(1);
}
//==== ====


//====Node makers====
static ASTNumberExpr* make_number_node(Arena* a, Token tok)
{
    ASTNumberExpr* n = arena_alloc(a, sizeof(ASTNumberExpr));
    n->base.type = AST_EXPR_NUMBER;
    n->token = tok;
    return n;
}


static ASTIdentExpr* make_ident_node(Arena* a, Token tok)
{
    ASTIdentExpr* id = arena_alloc(a, sizeof(ASTIdentExpr));
    id->base.type = AST_EXPR_IDENT;
    id->token = tok;
    return id;
}


static ASTBinaryExpr* make_binary_node(Arena* a, Token op, ASTNode* lhs, ASTNode* rhs)
{
    ASTBinaryExpr* bin = arena_alloc(a, sizeof(ASTBinaryExpr));
    bin->base.type = AST_EXPR_BINARY;
    bin->lhs = lhs;
    bin->rhs = rhs;
    bin->op = op;
    return bin;
}


static ASTVarDeclStmt* make_vardecl_node(Arena* a, Token name_tok, Token type_tok, ASTNode* init)
{
    ASTVarDeclStmt* decl = arena_alloc(a, sizeof(ASTVarDeclStmt));

    decl->base.type = AST_STMT_VARDECL;
    decl->name_token = name_tok;
    decl->type_token = type_tok;
    decl->initializer = init;

    return decl;
}
//==== ====



static ASTNode* parse_expression(Arena* a, Parser* p);

static ASTNode* parse_primary(Arena* a, Parser* p)
{
    if (check(p, TOK_NUMBER))
    {
        Token tok = advance(p);
        return (ASTNode*)make_number_node(a, tok);
    }

    if (check(p, TOK_IDENTIFIER))
    {
        Token tok = advance(p);
        return (ASTNode*)make_ident_node(a, tok);
    }
    
    if (check(p, TOK_LEFT_PAREN))
    {
        advance(p);
        ASTNode* expr = parse_expression(a, p);
        expect(p, TOK_RIGHT_PAREN);
        return expr;
    }

    fprintf(stderr, "Syntax error: unexpected token in line %d\n", p->current.line);
    exit(1);
}


static ASTNode* parse_factor(Arena* a, Parser* p)
{
    Token op;
    ASTNode* rhs;
    ASTNode* lhs = parse_primary(a, p);

    while (p->current.type == TOK_MULTIPLY || p->current.type == TOK_DIVIDE)
    {
        op = advance(p);
        rhs = parse_primary(a, p);
        lhs = (ASTNode*)make_binary_node(a, op, lhs, rhs);
    }
    return lhs;
}


static ASTNode* parse_expression(Arena* a, Parser* p)
{
    Token op;
    ASTNode* rhs;
    ASTNode* lhs = parse_factor(a, p);

    while (p->current.type == TOK_PLUS || p->current.type == TOK_MINUS)
    {
        op = advance(p);
        rhs = parse_factor(a, p);
        lhs = (ASTNode*)make_binary_node(a, op, lhs, rhs);
    }
    return lhs;
}



ASTNode* parse_declaration(Arena* a, Parser* p)
{
    expect(p, TOK_LET);
    Token name_tok = expect(p, TOK_IDENTIFIER);
    expect(p, TOK_COLON);
    Token type_tok = expect(p, TOK_INT);
    expect(p, TOK_ASSIGNMENT);
    ASTNode* init = parse_expression(a, p);
    expect(p, TOK_SEMICOLON);

    return (ASTNode*)make_vardecl_node(a, name_tok, type_tok, init);
}
