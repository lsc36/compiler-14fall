#ifndef PARSER_H
#define PARSER_H


Declaration parseDeclaration( FILE *source, Token token );
Declarations *parseDeclarations( FILE *source );
Expression *parseValue( FILE *source );
Expression *parseExpressionTail( FILE *source, Expression *lvalue );
Expression *parseExpression( FILE *source, Expression *lvalue );
Statement parseStatement( FILE *source, Token token );
Statements *parseStatements( FILE * source );

Program parser( FILE *source );
#endif