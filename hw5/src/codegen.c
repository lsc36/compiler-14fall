#include "codegen.h"

#define IDSTR(idNode) (idNode->semantic_value.identifierSemanticValue.identifierName)

FILE *outfile;

void codegenInit() {
    outfile = fopen("output.s", "w");
}

void codegenEnd() {
    fclose(outfile);
}

void emit(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(outfile, fmt, args);
    fprintf(outfile, "\n");
    va_end(args);
}

void genGlobalVarDecl(AST_NODE *varDeclListNode) {
    // TODO
}

void genBlock(AST_NODE *blockNode) {
    // TODO
}

void genFunctionDecl(AST_NODE *funcDeclNode) {
    AST_NODE *typeNode, *idNode, *paramListNode, *blockNode;
    typeNode = funcDeclNode->child;
    idNode = typeNode->rightSibling;
    paramListNode = idNode->rightSibling;
    blockNode = paramListNode->rightSibling;

    emit(".text");
    emit("_start_%s:", IDSTR(idNode));
    emit("str lr, [sp, #0]");
    emit("str fp, [sp, #-4]");
    emit("add fp, sp, #-4");
    emit("add sp, sp, #-8");

    genBlock(blockNode);

    emit("_end_%s:", IDSTR(idNode));
    emit("ldr lr, [fp, #4]");
    emit("add sp, fp, #4");
    emit("ldr fp, [fp, #0]");
    emit("bx lr");
}

void codegen(AST_NODE *root) {
    codegenInit();

    AST_NODE *child = root->child;
    if (child->nodeType == VARIABLE_DECL_LIST_NODE) {
        genGlobalVarDecl(child);
        child = child->rightSibling;
    }
    for (; child != NULL; child = child->rightSibling) {
        genFunctionDecl(child);
    }

    codegenEnd();
}
