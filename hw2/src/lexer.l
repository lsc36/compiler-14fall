%option noyywrap 
%{
#include <stdio.h>
#include "header.h" 
int linenumber;
symtab * lookup();
symtab * ptr;
void     insertID();
void 	 printSymTab();

void comment();
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
#define OP_ASSIGN   12
#define OP_OR       13
#define OP_AND      14
#define OP_NOT      15
#define OP_ADD      16
#define OP_SUB      17
#define OP_MUL      18
#define OP_DIV      19
#define OP_LT       20
#define OP_GT       21
#define OP_LEQ      22
#define OP_GEQ      23
#define OP_NEQ      24
#define OP_EQ       25
#define MK_LPAREN   26
#define MK_RPAREN   27
#define MK_LBRACE   28
#define MK_RBRACE   29
#define MK_LBRACK   30
#define MK_RBRACK   31
#define MK_SEMICOL  32
#define MK_COMMA    33
#define MK_DOT      34
#define ERROR       100

%}

letter   [A-Za-z]
digit    [0-9]
ID	 {letter}({letter}|{digit}|"_")*
WS	 [ \t]+
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
{Int_constant}  { /*return INT; */ }
{Float_constant}    { /*return FLOAT;*/ }
{ID}            { 
			ptr = lookup(yytext);
			if (ptr == NULL)
			     insertID(yytext);	
			else 
			     ptr->counter++;
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
{error}         printf("ERR \n");/* return ERROR; */

%%

main(int argc, char **argv)
{
 argc--; ++argv;
   if (argc > 0)
      yyin = fopen(argv[0], "r");
   else
      yyin = stdin;
   yylex();
   printSymTab();
}

