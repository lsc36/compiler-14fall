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
#define MK_LPAREN   14
#define MK_RPAREN   15
#define ERROR       100

%}

letter   [A-Za-z]
digit    [0-9]
ID	 {letter}({letter}|{digit}|"_")*
WS	 [ \t]+
Int_constant {digit}+

RWInt {Int_constant}
RWFloat {digit}+e(\+|-)?{digit}+|{digit}+(\.{digit}+)(e(\+|-)?{digit}+)?|({digit}+)?(\.{digit}+)(e(\+|-)?{digit}+)?

/* You need to define the following RE's 
Float_constant
String_constant
comment
({digit}+)(e(+|-)?{digit}+)|({digit}+(\.{digit}+)(e (+|-)?{digit}+)?)|({digit}+)?(\.{digit}+)(e(+|-)?{digit}+)?
*/

OP_assign        "="
OP_or   "||"

/* Other operators appear here */ 


newline  "\n"

DL_lparen "("
DL_rparen ")"
DL_lbrace "{"
DL_rbrace "}"
DL_comma  ","
DL_semicol ";"
DL_dot  "."

/* Other separators appear here */

error    .

%%

"/*"            {comment();}
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
{RWInt}         { /*return INT; */ }
{RWFloat}       { /*return FLOAT;*/ }
{ID}            { 
			ptr = lookup(yytext);
			if (ptr == NULL)
			     insertID(yytext);	
			else 
			     ptr->counter++;
		}

{OP_assign}     /* return OP_ASSIGN; */ 
{OP_or}         /* return OP_OR; */
{DL_lparen}     /* return MK_LPAREN; */
{DL_rparen}     /* return MK_RPAREN; */
{DL_lbrace}     /* return MK_LBRACE; */
{DL_rbrace}     /* return MK_RBRACE; */
{DL_comma}      {}
{DL_semicol}    {}
{newline}       linenumber += 1;
{error}         printf("ERR \n");/* return ERROR; */


%%

/* TODO: infinite memory for buf, or reuse buffer */

void comment() {
	char buffer[4096];
	buffer[0] = '/';
	buffer[1] = '*';
	int count = 2;
	while (1) {
		char c = input();
		if ((c == '/' && buffer[count - 1] == '*')) {
			buffer[count++] = c;
			buffer[count] = '\0';
			break;
		}
		else if (c == EOF){
			break;
		}
		else {
			buffer[count++] = c;
		}
	}
	printf("%s\n", buffer);
}

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

