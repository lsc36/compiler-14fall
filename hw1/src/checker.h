#ifndef CHECKER_H
#define CHECKER_H

#include "type.h"

void convertType( Expression * old, DataType type );
DataType generalize( Expression *left, Expression *right );
DataType lookup_table( SymbolTable *table, char c );
void checkexpression( Expression * expr, SymbolTable * table );
void checkstmt( Statement *stmt, SymbolTable * table );
void check( Program *program, SymbolTable * table);

#endif