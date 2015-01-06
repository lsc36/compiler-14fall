#include <stdio.h>
#include "header.h"
#include "symbolTable.h"
#include "offset.h"

int OffsetInFunction;

char *getFunctionName(AST_NODE *funcDeclNode){
	return funcDeclNode->child->rightSibling->semantic_value.identifierSemanticValue.identifierName;
}

char *getIdName(AST_NODE *idNode){
	return idNode->semantic_value.identifierSemanticValue.identifierName;
}

SymbolTableEntry *getSymboltableEntry(AST_NODE *node) {
	return node->semantic_value.identifierSemanticValue.symbolTableEntry;
}

int isFunctionDecl(AST_NODE *node) {
	return (node->nodeType == DECLARATION_NODE && node->semantic_value.declSemanticValue.kind == FUNCTION_DECL);
}

int isBlock(AST_NODE *node) {
	return (node->nodeType == BLOCK_NODE);
}

int isParamList(AST_NODE *node) {
	return (node->nodeType == PARAM_LIST_NODE);
}

void countParamOffset(AST_NODE *paramListNode){
	int offset = 8;
	AST_NODE *paramNode = paramListNode->child;
	for (; paramNode != NULL; paramNode = paramNode->rightSibling, offset += 4) {
		AST_NODE *id = paramNode->child->rightSibling;
		SymbolTableEntry *entry = getSymboltableEntry(id);
		entry->offset = offset;
		/* fprintf(stderr, "parameter %s offset is %d\n", getIdName(id), offset); */
	}
}

// 計算在block內的offset
void countVariableOffset(AST_NODE *blockNode) {
	
	AST_NODE *decl;
	if (blockNode->child->nodeType == VARIABLE_DECL_LIST_NODE) {
		decl = blockNode->child->child;
	} else {return;}

	for (; decl != NULL; decl = decl->rightSibling) {
		if (decl->semantic_value.declSemanticValue.kind == VARIABLE_DECL) {
			AST_NODE *id = decl->child->rightSibling;
			for (;id != NULL; id = id->rightSibling) {
				SymbolTableEntry *entry = getSymboltableEntry(id);
				entry->offset = OffsetInFunction;
				OffsetInFunction -= 4;
				/* fprintf(stderr, "variable %s offset is %d\n", getIdName(id), entry->offset); */
			}
		}
	}

}

void traverseNode(AST_NODE *node) {

	AST_NODE *child = node->child;

	if (isFunctionDecl(node)) {
		OffsetInFunction = -4;
		/* fprintf(stderr, "capture function %s\n", getFunctionName(node)); */
		for (; child != NULL; child = child->rightSibling) {
			traverseNode(child);
		}
	}
	else if (isParamList(node)) {
		countParamOffset(node);
		return;
	}
	else if (isBlock(node)) {
		int originOffset = OffsetInFunction;
		countVariableOffset(node);
		for (; child != NULL; child = child->rightSibling) {
			traverseNode(child);
		}
		OffsetInFunction = originOffset;
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

