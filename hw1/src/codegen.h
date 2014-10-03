#ifndef CODEGEN_H
#define CODEGEN_H

#include "header.h"

void fprint_op( FILE *target, ValueType op );
void fprint_expr( FILE *target, Expression *expr );
void gencode( Program prog, FILE * target );

#endif
