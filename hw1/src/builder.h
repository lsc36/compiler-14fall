#ifndef BUILDER_H
#define BUILDER_H

#include "type.h"

void InitializeTable( SymbolTable *table );
void add_table( SymbolTable *table, char c, DataType t );
SymbolTable build( Program program );

#endif
