#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#include"header.h"

#define TABLE_SIZE	256

symtab * hash_table[TABLE_SIZE];
extern int linenumber;
int n_symbols;

int HASH(char * str){
	int idx=0;
	while(*str){
		idx = idx << 1;
		idx+=*str;
		str++;
	}	
	return (idx & (TABLE_SIZE-1));
}

/*returns the symbol table entry if found else NULL*/

symtab * lookup(char *name){
	int hash_key;
	symtab* symptr;
	if(!name)
		return NULL;
	hash_key=HASH(name);
	symptr=hash_table[hash_key];

	while(symptr){
		if(!(strcmp(name,symptr->lexeme)))
			return symptr;
		symptr=symptr->front;
	}
	return NULL;
}


void insertID(char *name){
	int hash_key;
	symtab* ptr;
	symtab* symptr=(symtab*)malloc(sizeof(symtab));	
	
	hash_key=HASH(name);
	ptr=hash_table[hash_key];
	
	if(ptr==NULL){
		/*first entry for this hash_key*/
		hash_table[hash_key]=symptr;
		symptr->front=NULL;
		symptr->back=symptr;
	}
	else{
		symptr->front=ptr;
		ptr->back=symptr;
		symptr->back=symptr;
		hash_table[hash_key]=symptr;	
	}
	
	strcpy(symptr->lexeme,name);
	symptr->line=linenumber;
	symptr->counter=1;

	n_symbols++;
}

void printSym(symtab* ptr) 
{
	    printf(" Name = %s \n", ptr->lexeme);
	    printf(" References = %d \n", ptr->counter);
}

void printSymTab()
{
    int i;
	printf("----- Symbol Table ---------\n");
	for (i=0; i<TABLE_SIZE; i++)
	{
		symtab* symptr;
		symptr = hash_table[i];
		while (symptr != NULL)
		{
			printf("====>  index = %d \n", i);
			printSym(symptr);
			symptr=symptr->front;
		}
	}
}

int comp_symbol_name(const void *a, const void *b)
{
	return strcmp((*(symtab**)a)->lexeme, (*(symtab**)b)->lexeme);
}

void printFreq()
{
	int i, pos;
	symtab **l_symptr;
	puts("Frequency of identifiers:");
	l_symptr = (symtab**)malloc(n_symbols * sizeof(symtab*));
	pos = 0;
	for (i = 0; i < TABLE_SIZE; i++)
	{
		symtab *symptr;
		for (symptr = hash_table[i]; symptr != NULL; symptr = symptr->front)
			l_symptr[pos++] = symptr;
	}
	qsort(l_symptr, n_symbols, sizeof(symtab*), comp_symbol_name);
	for (i = 0; i < n_symbols; i++)
		printf("%-15s %d\n", l_symptr[i]->lexeme, l_symptr[i]->counter);
	free(l_symptr);
}
