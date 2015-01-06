#include <stdio.h>
#include "header.h"
#include "symbolTable.h"
#include "offset.h"

char *getFunctionName(AST_NODE *funcDeclNode){
	return funcDeclNode->child->rightSibling->semantic_value.identifierSemanticValue.identifierName;
}


void traverseNode(AST_NODE *node) {

	AST_NODE *child = node->child;

	if (node->nodeType == DECLARATION_NODE && node->semantic_value.declSemanticValue.kind == FUNCTION_DECL) {
		printf("capture function %s\n", getFunctionName(node));
	}
	else {
		for (; child != NULL; child = child->rightSibling) {
			traverseNode(child);
		}
	}
}


void offsetAnalysis(AST_NODE *program) {
	traverseNode(program);
}

