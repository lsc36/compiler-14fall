#ifndef PARSER_H
#define PARSER_H

#include "type.h"

Declaration makeDeclarationNode( Token declare_type, Token identifier );
Declarations *makeDeclarationTree( Declaration decl, Declarations *decls );
Statement makeAssignmentNode( char *id, Expression *v, Expression *expr_tail );
Statement makePrintNode( char *id );
Statements *makeStatementTree( Statement stmt, Statements *stmts );


Declaration parseDeclaration( FILE *source, Token token );
Declarations *parseDeclarations( FILE *source );
Expression *parsePosValue(FILE *source);
Expression *parseExpressionTail( FILE *source, Expression *lvalue );
Expression *parseExpression( FILE *source, Expression *lvalue );
Statement parseStatement( FILE *source, Token token );
Statements *parseStatements( FILE * source );

Program parser( FILE *source );
#endif