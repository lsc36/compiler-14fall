#include <stdio.h>
#include <string.h>
#include "type.h"
#include "builder.h"


/********************************************************
  Build symbol table
 *********************************************************/
void InitializeTable( SymbolTable *table )
{
    int i;

    for(i = 0 ; i < 26; i++) {
        table->table[i] = Notype;
        table->name[i][0] = '\0';
    }
    table->count = 0;
}

void add_table( SymbolTable *table, char *s, DataType t )
{
    int i;
    for (i = 0; i < table->count; i++) {
        if(strcmp(s, table->name[i]) == 0) {
            printf("Error : id %s has been declared\n", s);//error
            //exit(1);
        }
    }
    table->count++;
    strcpy(table->name[i], s);
    table->table[i] = t;
}

SymbolTable build( Program program )
{
    SymbolTable table;
    Declarations *decls = program.declarations;
    Declaration current;

    InitializeTable(&table);

    while(decls !=NULL){
        current = decls->first;
        add_table(&table, current.name, current.type);
        decls = decls->rest;
    }

    return table;
}
