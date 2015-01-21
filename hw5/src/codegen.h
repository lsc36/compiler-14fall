#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "header.h"
#include "symbolTable.h"

#define IDSTR(idNode) (idNode->semantic_value.identifierSemanticValue.identifierName)
#define IDKIND(idNode) (idNode->semantic_value.identifierSemanticValue.kind)
#define IDSYM(idNode) (idNode->semantic_value.identifierSemanticValue.symbolTableEntry)
#define DECLKIND(declNode) (declNode->semantic_value.declSemanticValue.kind)
#define STMTKIND(stmtNode) (stmtNode->semantic_value.stmtSemanticValue.kind)
#define EXPRKIND(exprNode) (exprNode->semantic_value.exprSemanticValue.kind)
#define EXPRBINOP(exprNode) (exprNode->semantic_value.exprSemanticValue.op.binaryOp)
#define EXPRUNIOP(exprNode) (exprNode->semantic_value.exprSemanticValue.op.unaryOp)
#define EXPRCONSTEVAL(exprNode) (exprNode->semantic_value.exprSemanticValue.isConstEval)
#define EXPRCONSTU(exprNode) (exprNode->semantic_value.exprSemanticValue.constEvalValue)
#define CONSTTYPE(constNode) (constNode->semantic_value.const1->const_type)
#define CONSTU(constNode) (constNode->semantic_value.const1->const_u)
#define SYMARRPROP(symtabEntry) (symtabEntry->attribute->attr.typeDescriptor->properties.arrayProperties)
#define SYMFUNCSIGN(symtabEntry) (symtabEntry->attribute->attr.functionSignature)

typedef enum REGISTER {
    R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, SP, LR, PC,
    S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15,
    S16, S17, S18, S19, S20, S21, S22, S23, S24, S25, S26, S27, S28, S29, S30, S31
} REGISTER;

void emit(char *fmt, ...);
int calcBlockOffset(AST_NODE *blockNode);

REGISTER allocateRegister(SymbolTableEntry *sym);

void genIntBinOp(BINARY_OPERATOR op, REGISTER dst, REGISTER lhs, REGISTER rhs);
void genIntUniOp(UNARY_OPERATOR op, REGISTER dst, REGISTER lhs);
void genFloatBinOp(BINARY_OPERATOR op, REGISTER dst, REGISTER lhs, REGISTER rhs);
void genFloatUniOp(UNARY_OPERATOR op, REGISTER dst, REGISTER lhs);
REGISTER genExpr(AST_NODE *node);
REGISTER genIntExpr(AST_NODE *node);
REGISTER genFloatExpr(AST_NODE *node);
void genGlobalVarDecl(AST_NODE *varDeclListNode);
void genFuncCall(AST_NODE *funcCallStmtNode);
void genAssign(AST_NODE *stmtNode);
void genStmt(AST_NODE *stmtNode);
void genStmtList(AST_NODE *stmtListNode);
void genBlock(AST_NODE *blockNode);
void genFunctionDecl(AST_NODE *funcDeclNode);
void genCalleeSaveRestore();
void genReadStub();

void codegen(AST_NODE *root);

#endif
