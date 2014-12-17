#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolTable.h"

extern SymbolTable symbolTable;

void print_symbloTable();

void print_title(char *s) {
	printf("-----------------------%s--------------------------\n", s);
}

void error_msg(char *s) {
	printf("\e[31m%s\e[0m\n", s);
}

void pass_msg() {
	printf("\e[32mpass test!!!!\e[0m");
}

void enter_and_retrieve() {
	print_title("run function entr_and_retrieve");
	//printf("run function entr_and_retrieve\n");
	
	initializeSymbolTable();
	enterSymbol("a", NULL);
	SymbolTableEntry *a = retrieveSymbol("a");
	SymbolTableEntry *b = retrieveSymbol("b");
	
	int OK = 1;
	if (strcmp(a->name, "a") != 0) {
		OK = 0;
		error_msg("cannot retrieve symbol just entered");
	}
	if (b != NULL) {
		OK = 0;
		error_msg("retrieve not entered symbol\n");
	}
	symbolTableEnd();
	
	if (OK) {pass_msg();}
}

int main(int argc, char *argv[]) {
	enter_and_retrieve();
}