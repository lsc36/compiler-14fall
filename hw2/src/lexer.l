%option noyywrap
%{
#include <stdio.h>
#include "header.h"
int linenumber = 1;
symtab * lookup();
symtab * ptr;
void     insertID();
void     printSymTab();
void     printFreq();

#define RETURN      1
#define TYPEDEF     2
#define IF          3
#define ELSE        4
#define FOR         5
#define STRUCT      6
#define UNION       7
#define VOID        8
#define WHILE       9
#define INT         10
#define FLOAT       11
#define STRING      12
#define OP_ASSIGN   21
#define OP_OR       22
#define OP_AND      23
#define OP_NOT      24
#define OP_ADD      25
#define OP_SUB      26
#define OP_MUL      27
#define OP_DIV      28
#define OP_LT       29
#define OP_GT       30
#define OP_LEQ      31
#define OP_GEQ      32
#define OP_NEQ      33
#define OP_EQ       34
#define MK_LPAREN   41
#define MK_RPAREN   42
#define MK_LBRACE   43
#define MK_RBRACE   44
#define MK_LBRACK   45
#define MK_RBRACK   46
#define MK_SEMICOL  47
#define MK_COMMA    48
#define MK_DOT      49
#define ERROR       100

%}

letter   [A-Za-z]
digit    [0-9]
ID       {letter}({letter}|{digit}|"_")*
WS       [ \t]+
Int_constant {digit}+
Float_constant {digit}+e(\+|-)?{digit}+|{digit}+(\.{digit}+)(e(\+|-)?{digit}+)?|({digit}+)?(\.{digit}+)(e(\+|-)?{digit}+)?
String_constant \"(\\.|[^\\"])*\"

comment \/\*(\*[^\/]|[^\*])*\*\/
newline  "\n"
error    .

%%

{comment}       { puts(yytext); }
{WS}            {}
"return"        { /* return RETURN; */ }
"typedef"       { /* return TYPEDEF; */ }
"if"            { /* return IF; */ }
"else"          { /* return ELSE; */ }
"for"           { /* return FOR; */ }
"struct"        { /* return STRUCT; */ }
"union"         { /* return UNION; */ }
"void"          { /* return VOID; */ }
"while"         { /* return WHILE; */ }
"int"           { /* return INT; */ }
"float"         { /* return FLOAT; */ }
{Int_constant}  {}
{Float_constant}    {}
{String_constant}   {}
{ID}            {
    ptr = lookup(yytext);
    if (ptr == NULL) insertID(yytext);
    else ptr->counter++;
}

"="             /* return OP_ASSIGN; */
"||"            /* return OP_OR; */
"&&"            /* return OP_AND; */
"!"             /* return OP_NOT; */
"+"             /* return OP_ADD; */
"-"             /* return OP_SUB; */
"*"             /* return OP_MUL; */
"/"             /* return OP_DIV; */
"<"             /* return OP_LT; */
">"             /* return OP_GT; */
"<="            /* return OP_LEQ; */
">="            /* return OP_GEQ; */
"!="            /* return OP_NEQ; */
"=="            /* return OP_EQ; */
"("             /* return MK_LPAREN; */
")"             /* return MK_RPAREN; */
"{"             /* return MK_LBRACE; */
"}"             /* return MK_RBRACE; */
"["             /* return MK_LBRACK; */
"]"             /* return MK_RBRACK; */
";"             /* return MK_SEMICOL; */
","             /* return MK_COMMA; */
"."             /* return MK_DOT; */
{newline}       linenumber += 1;
{error}         {
    printf("error on line %d: \"%s\"\n", linenumber, yytext);
    /* return ERROR; */
    exit(1);
}

%%

main(int argc, char **argv)
{
    argc--; ++argv;
    if (argc > 0)
        yyin = fopen(argv[0], "r");
    else
        yyin = stdin;
    yylex();
    /* printSymTab(); */
    printFreq();
}

