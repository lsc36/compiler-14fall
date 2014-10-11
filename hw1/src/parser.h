#ifndef PARSER_H
#define PARSER_H

#include "type.h"

Declaration makeDeclarationNode( Token declare_type, Token identifier );
Declarations *makeDeclarationTree( Declaration decl, Declarations *decls );
Statement makeAssignmentNode(char *id, Expression *expr);
Statement makePrintNode( char *id );
Statements *makeStatementTree( Statement stmt, Statements *stmts );


Declaration parseDeclaration( FILE *source, Token token );
Declarations *parseDeclarations( FILE *source );
Expression *parseValue(FILE *source);
Expression *parsePosValue(Token token);
Expression *parseTerm(FILE *source, Expression *lvalue) ;
Expression *parseExpression(FILE *source, Expression *lvalue);
Statement parseStatement( FILE *source, Token token );
Statements *parseStatements( FILE * source );

Program parser( FILE *source );
#endif