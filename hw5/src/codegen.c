#include "codegen.h"

#define IDSTR(idNode) (idNode->semantic_value.identifierSemanticValue.identifierName)
#define IDKIND(idNode) (idNode->semantic_value.identifierSemanticValue.kind)
#define IDSYM(idNode) (idNode->semantic_value.identifierSemanticValue.symbolTableEntry)
#define DECLKIND(declNode) (declNode->semantic_value.declSemanticValue.kind)
#define SYMARRPROP(symtabEntry) (symtabEntry->attribute->attr.typeDescriptor->properties.arrayProperties)

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

void genStmtList(AST_NODE *stmtListNode) {
    // TODO
}

void genBlock(AST_NODE *blockNode) {
    AST_NODE *child = blockNode->child;
    int stackSize = 0;
    if (child != NULL && child->nodeType == VARIABLE_DECL_LIST_NODE) {
        AST_NODE *declNode = child->child;
        for (; declNode != NULL; declNode = declNode->rightSibling) {
            AST_NODE *typeNode, *idNode;
            switch (DECLKIND(declNode)) {
            case VARIABLE_DECL:
                typeNode = declNode->child;
                idNode = typeNode->rightSibling;
                for (; idNode != NULL; idNode = idNode->rightSibling) {
                    int product = 1, i;
                    switch (IDKIND(idNode)) {
                    case NORMAL_ID:
                        stackSize += 4;
                        break;
                    case ARRAY_ID:
                        for (i = 0; i < SYMARRPROP(IDSYM(idNode)).dimension; i++) {
                            product *= SYMARRPROP(IDSYM(idNode)).sizeInEachDimension[i];
                        }
                        stackSize += 4 * product;
                        break;
                    case WITH_INIT_ID:
                        // TODO assign init value
                        stackSize += 4;
                        break;
                    default:
                        ;
                    }
                }
                break;
            default:
                ;
            }
        }
        if (stackSize > 0) {
            emit("add sp, sp, #-%d", stackSize);
        }
        child = child->rightSibling;
    }
    if (child != NULL) {
        genStmtList(child);
    }
    if (stackSize > 0) {
        emit("add sp, sp, #%d", stackSize);
    }
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
