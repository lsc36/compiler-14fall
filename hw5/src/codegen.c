#include "codegen.h"

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
    va_end(args);
}

void genGlobalVarDecl(AST_NODE *varDeclListNode) {
    // TODO
}

void genFunctionDecl(AST_NODE *funcDeclNode) {
    // TODO
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
