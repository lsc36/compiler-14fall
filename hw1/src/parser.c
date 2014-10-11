#include <stdio.h>
#include <string.h>
#include "type.h"
#include "scanner.h"
#include "parser.h"


/*********************************************************************
Build AST
**********************************************************************/

Declaration makeDeclarationNode( Token declare_type, Token identifier )
{
    Declaration tree_node;

    switch(declare_type.type){
        case FloatDeclaration:
            tree_node.type = Float;
            break;
        case IntegerDeclaration:
            tree_node.type = Int;
            break;
        default:
            break;
    }
    strcpy(tree_node.name, identifier.tok);

    return tree_node;
}

Declarations *makeDeclarationTree( Declaration decl, Declarations *decls )
{
    Declarations *new_tree = (Declarations *)malloc( sizeof(Declarations) );
    new_tree->first = decl;
    new_tree->rest = decls;

    return new_tree;
}


Statement makeAssignmentNode(char *id, Expression *expr)
{
    Statement stmt;
    AssignmentStatement assign;

    stmt.type = Assignment;
    strcpy(assign.id, id);
    assign.expr = expr;
    stmt.stmt.assign = assign;

    return stmt;
}

Statement makePrintNode( char *id )
{
    Statement stmt;
    stmt.type = Print;
    strcpy(stmt.stmt.variable, id);

    return stmt;
}

Statements *makeStatementTree( Statement stmt, Statements *stmts )
{
    Statements *new_tree = (Statements *)malloc( sizeof(Statements) );
    new_tree->first = stmt;
    new_tree->rest = stmts;

    return new_tree;
}



/********************************************************
  Parsing
 *********************************************************/
Declaration parseDeclaration( FILE *source, Token token )
{
    Token token2;
    switch(token.type){
        case FloatDeclaration:
        case IntegerDeclaration:
            token2 = scanner(source);
            if (strcmp(token2.tok, "f") == 0 ||
                    strcmp(token2.tok, "i") == 0 ||
                    strcmp(token2.tok, "p") == 0) {
                printf("Syntax Error: %s cannot be used as id\n", token2.tok);
                exit(1);
            }
            return makeDeclarationNode( token, token2 );
        default:
            printf("Syntax Error: Expect Declaration %s\n", token.tok);
            exit(1);
    }
}

Declarations *parseDeclarations( FILE *source )
{
    Token token = scanner(source);
    Declaration decl;
    Declarations *decls;
    switch(token.type){
        case FloatDeclaration:
        case IntegerDeclaration:
            decl = parseDeclaration(source, token);
            decls = parseDeclarations(source);
            return makeDeclarationTree( decl, decls );
        case PrintOp:
        case Alphabet:
            for (int i = strlen(token.tok) - 1; i >= 0; i--) {
                ungetc(token.tok[i], source);
            }
            return NULL;
        case EOFsymbol:
            return NULL;
        default:
            printf("Syntax Error: Expect declarations %s\n", token.tok);
            exit(1);
    }
}

Expression *parseValue(FILE *source)
{
    Token token = scanner(source);
    Expression *value;

    switch (token.type){
        case Alphabet:
        case IntValue:
        case FloatValue:
            return parsePosValue(token);
        case MinusOp:
            value = (Expression *)malloc( sizeof(Expression) );
            Token next_token = scanner(source);
            value->leftOperand = parsePosValue(next_token);
            value->rightOperand = NULL;
            (value->v).type = NegNode;
            (value->v).val.op = Neg;
            return value;
        default:
            printf("Syntax Error: Expect Neg symbol or PosValue, but %s", token.tok);
            exit(1);
    }
}

Expression *parsePosValue(Token token)
{
    Expression *value = (Expression *)malloc( sizeof(Expression) );
    value->leftOperand = value->rightOperand = NULL;

    switch(token.type){
        case Alphabet:
            (value->v).type = Identifier;
            strcpy((value->v).val.id, token.tok);
            break;
        case IntValue:
            (value->v).type = IntConst;
            (value->v).val.ivalue = atoi(token.tok);
            break;
        case FloatValue:
            (value->v).type = FloatConst;
            (value->v).val.fvalue = (float)atof(token.tok);
            break;
        default:
            printf("Syntax Error: Expect Identifier or a Number %s\n", token.tok);
            exit(1);
    }

    return value;
}

Expression *parseTerm(FILE *source, Expression *lvalue)
{
    Token token = scanner(source);
    Expression *expr;
    switch (token.type){
        case MulOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = MulNode;
            (expr->v).val.op = Mul;
            expr->leftOperand = lvalue;
            expr->rightOperand = parseValue(source);
            return parseTerm(source, expr);
        case DivOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = DivNode;
            (expr->v).val.op = Div;
            expr->leftOperand = lvalue;
            expr->rightOperand = parseValue(source);
            return parseTerm(source, expr);
        case PlusOp:
        case MinusOp:
        case Alphabet:
        case PrintOp:
            for (int i = strlen(token.tok) - 1; i >= 0; i--) {
                ungetc(token.tok[i], source);
            }
            return lvalue;
        default:
            printf("In function parseTerm\n");
            printf("Syntax Error: Expect a numeric value or an identifier %s\n", token.tok);
            exit(1);
    }

}

// 跟parseExpression差別只在遇到Alphabet, PrintOp的返回值
// 好多重複程式碼
Expression *parseExpression(FILE *source, Expression *lvalue)
{
    Token token = scanner(source);
    Expression *expr;
    Expression *val;

    switch(token.type){
        case PlusOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = PlusNode;
            (expr->v).val.op = Plus;
            expr->leftOperand = lvalue;
            val = parseValue(source);
            expr->rightOperand = parseTerm(source, val);
            return parseExpression(source, expr);
        case MinusOp:
            expr = (Expression *)malloc( sizeof(Expression) );
            (expr->v).type = MinusNode;
            (expr->v).val.op = Minus;
            expr->leftOperand = lvalue;
            val = parseValue(source);
            expr->rightOperand = parseTerm(source, val);
            return parseExpression(source, expr);
        case Alphabet:
        case PrintOp:
            for (int i = strlen(token.tok) - 1; i >= 0; i--) {
                ungetc(token.tok[i], source);
            }
            return lvalue;
        case EOFsymbol:
            return lvalue;
        default:
            printf("In function parseExpression\n");
            printf("Syntax Error: Expect a numeric value or an identifier %s\n", token.tok);
            exit(1);
    }
}

Statement parseStatement( FILE *source, Token token )
{
    Token next_token;
    Expression *value, *term, *expr;

    switch(token.type){
        case Alphabet:
            next_token = scanner(source);
            if(next_token.type == AssignmentOp){
                value = parseValue(source);
                term = parseTerm(source, value);
                expr = parseExpression(source, term);
                return makeAssignmentNode(token.tok, expr);
            }
            else{
                printf("Syntax Error: Expect an assignment op %s\n", next_token.tok);
                exit(1);
            }
        case PrintOp:
            next_token = scanner(source);
            if(next_token.type == Alphabet)
                return makePrintNode(next_token.tok);
            else{
                printf("Syntax Error: Expect an identifier %s\n", next_token.tok);
                exit(1);
            }
            //break;
        default:
            printf("Syntax Error: Expect a statement %s\n", token.tok);
            exit(1);
    }
}

Statements *parseStatements( FILE * source )
{

    Token token = scanner(source);
    Statement stmt;
    Statements *stmts;

    switch(token.type){
        case Alphabet:
        case PrintOp:
            stmt = parseStatement(source, token);
            stmts = parseStatements(source);
            return makeStatementTree(stmt , stmts);
        case EOFsymbol:
            return NULL;
        default:
            printf("Syntax Error: Expect statements %s\n", token.tok);
            exit(1);
    }
}

Program parser( FILE *source )
{
    Program program;

    program.declarations = parseDeclarations(source);
    program.statements = parseStatements(source);

    return program;
}
