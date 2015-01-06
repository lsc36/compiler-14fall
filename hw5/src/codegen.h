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

typedef enum REGISTER {
    R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, SP, LR, PC,
    S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15,
    S16, S17, S18, S19, S20, S21, S22, S23, S24, S25, S26, S27, S28, S29, S30, S31
} REGISTER;

const char *REG[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "sp", "lr", "pc",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "s12", "s13", "s14", "s15",
    "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23", "s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31"
};

void emit(char *fmt, ...);
int calcBlockOffset(AST_NODE *blockNode);

REGISTER genExpr(AST_NODE *node);
void genGlobalVarDecl(AST_NODE *varDeclListNode);
void genFuncCall(AST_NODE *funcCallStmtNode);
void genStmtList(AST_NODE *stmtListNode);
void genBlock(AST_NODE *blockNode);
void genFunctionDecl(AST_NODE *funcDeclNode);

void codegen(AST_NODE *root);

#endif
