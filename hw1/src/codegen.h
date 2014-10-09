#ifndef CODEGEN_H
#define CODEGEN_H

#include "type.h"

void fprint_op( FILE *target, ValueType op );
char find_register(char *s, SymbolTable *table);
void fprint_expr(FILE *target, Expression *expr, SymbolTable *table);
void gencode( Program prog, FILE * target , SymbolTable *table);

#endif
