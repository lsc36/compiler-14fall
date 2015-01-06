#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <stdio.h>
#include <stdarg.h>
#include "header.h"
#include "symbolTable.h"

#define IDSTR(idNode) (idNode->semantic_value.identifierSemanticValue.identifierName)
#define IDKIND(idNode) (idNode->semantic_value.identifierSemanticValue.kind)
#define IDSYM(idNode) (idNode->semantic_value.identifierSemanticValue.symbolTableEntry)
#define DECLKIND(declNode) (declNode->semantic_value.declSemanticValue.kind)
#define STMTKIND(stmtNode) (stmtNode->semantic_value.stmtSemanticValue.kind)
#define SYMARRPROP(symtabEntry) (symtabEntry->attribute->attr.typeDescriptor->properties.arrayProperties)

void emit(char *fmt, ...);
int calcBlockOffset(AST_NODE *blockNode);

void genGlobalVarDecl(AST_NODE *varDeclListNode);
void genStmtList(AST_NODE *stmtListNode);
void genBlock(AST_NODE *blockNode);
void genFunctionDecl(AST_NODE *funcDeclNode);

void codegen(AST_NODE *root);

#endif
