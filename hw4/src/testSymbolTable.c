#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolTable.h"

extern SymbolTable symbolTable;

SymbolAttribute *make_int_attr(int n) {
	SymbolAttribute *ans = (SymbolAttribute *)malloc(sizeof(SymbolAttribute *));
	ans->attributeKind = VARIABLE_ATTRIBUTE;
	ans->attr.typeDescriptor = (TypeDescriptor*)malloc(sizeof(TypeDescriptor*));
	ans->attr.typeDescriptor->kind = SCALAR_TYPE_DESCRIPTOR;
	ans->value.int_value = n;
	return ans;
}

void print_symbloTable();

void print_title(char *s) {
	printf("-----------------------%s--------------------------\n", s);
}

void error_msg(char *s) {
	printf("\e[31m%s\e[0m\n", s);
}

void pass_msg() {
	printf("\e[32mpass test!!!!\e[0m\n");
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
		error_msg("retrieve not entered symbol");
	}
	symbolTableEnd();
	
	if (OK) {pass_msg();}
}

void enter_remove_retrieve() {
	print_title("run function enter_remove_retrieve");
	
	initializeSymbolTable();
	enterSymbol("a", NULL);
	enterSymbol("c", NULL);
	SymbolTableEntry *a = retrieveSymbol("a");
	SymbolTableEntry *b = retrieveSymbol("b");
	SymbolTableEntry *c = retrieveSymbol("c");
	removeSymbol("c");
	c = retrieveSymbol("c");
	
	int OK = 1;
	if (strcmp(a->name, "a") != 0) {
		OK = 0;
		error_msg("cannot retrieve symbol just entered");
	}
	if (b != NULL) {
		OK = 0;
		error_msg("retrieve not entered symbol");
	}
	if (c != NULL) {
		OK = 0;
		error_msg("remove not work");
	}
	symbolTableEnd();
	
	if (OK) {pass_msg();}
}

void scope_open_close() {
	print_title("run function scope_open_close");
	SymbolTableEntry *a, *b, *c;
	int OK = 1;
	initializeSymbolTable();
	enterSymbol("a", make_int_attr(1));
	openScope();
	enterSymbol("a", make_int_attr(2));
	enterSymbol("b", make_int_attr(3));
	a = retrieveSymbol("a");
	b = retrieveSymbol("b");
	if (a->attribute->value.int_value != 2 || b->attribute->value.int_value != 3) {
		OK = 0;
		printf("a->attribute->value.int_value is %d\n", a->attribute->value.int_value);
		printf("b->attribute->value.int_value is %d\n", b->attribute->value.int_value);
		error_msg("get symbol in scope 2 error");
	}
	closeScope();
	a = retrieveSymbol("a");
	b = retrieveSymbol("b");
	if (a->attribute->value.int_value != 1 || b != NULL) {
		OK = 0;
		printf("a->attribute->value.int_value is %d\n", a->attribute->value.int_value);
		printf("b->attribute->value.int_value is %d\n", b->attribute->value.int_value);
		error_msg("get symbol in scope 1 error");
	}
	symbolTableEnd();
	if (OK) {pass_msg();}
}

int main(int argc, char *argv[]) {
	enter_and_retrieve();
	enter_remove_retrieve();
	scope_open_close();
}