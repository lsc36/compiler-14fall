#include "codegen.h"

FILE *outfile;
AST_NODE *curFuncIdNode;

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

REGISTER genExpr(AST_NODE *node) {
    switch (node->nodeType) {
    case EXPR_NODE:
        // TODO
        break;
    case CONST_VALUE_NODE:
        // TODO
        break;
    case IDENTIFIER_NODE:
        switch (IDKIND(node)) {
        case NORMAL_ID:
            // TODO
            break;
        case ARRAY_ID:
            // TODO
            break;
        default:
            ;
        }
        break;
    case STMT_NODE:  // FUNCTION_CALL_STMT
        genFuncCall(node);
        return R0;
    default:
        ;
    }
    // TODO
    return R4;
}

void genGlobalVarDecl(AST_NODE *varDeclListNode) {
    // TODO
}

void genFuncCall(AST_NODE *funcCallStmtNode) {
    // TODO push parameters onto stack
    emit("bl _start_%s", IDSTR(funcCallStmtNode->child));
}

void genStmtList(AST_NODE *stmtListNode) {
    AST_NODE *child = stmtListNode->child;
    for (; child != NULL; child = child->rightSibling) {
        if (child->nodeType == STMT_NODE) {
            switch (STMTKIND(child)) {
            case WHILE_STMT:
                // TODO
                break;
            case FOR_STMT:
                // TODO
                break;
            case ASSIGN_STMT:
                // TODO
                break;
            case IF_STMT:
                // TODO
                break;
            case FUNCTION_CALL_STMT:
                genFuncCall(child);
                break;
            case RETURN_STMT:
                if (child->child->nodeType != NUL_NODE) {
                    REGISTER result = genExpr(child->child);
                    if (result != R0) emit("mov r0, %s", REG[result]);
                }
                emit("b _end_%s", IDSTR(curFuncIdNode));
                break;
            default:
                ;
            }
        } else if (child->nodeType == BLOCK_NODE) {
            genBlock(child);
        } else if (child->nodeType == NUL_NODE) {
            // do nothing
        }
    }
}

void genBlock(AST_NODE *blockNode) {
    AST_NODE *child = blockNode->child;
    if (child != NULL && child->nodeType == VARIABLE_DECL_LIST_NODE) {
        AST_NODE *declNode = child->child;
        for (; declNode != NULL; declNode = declNode->rightSibling) {
            if (DECLKIND(declNode) == VARIABLE_DECL) {
                AST_NODE *typeNode, *idNode;
                typeNode = declNode->child;
                idNode = typeNode->rightSibling;
                for (; idNode != NULL; idNode = idNode->rightSibling) {
                    if (IDKIND(idNode) == WITH_INIT_ID) {
                        // TODO assign init value
                    }
                }
            }
        }
        child = child->rightSibling;
    }
    if (child != NULL) {
        genStmtList(child);
    }
}

int calcBlockOffset(AST_NODE *blockNode) {
    AST_NODE *child = blockNode->child;
    int baseSize = 0, innerSize = 0;
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
                        baseSize += 4;
                        break;
                    case ARRAY_ID:
                        for (i = 0; i < SYMARRPROP(IDSYM(idNode)).dimension; i++) {
                            product *= SYMARRPROP(IDSYM(idNode)).sizeInEachDimension[i];
                        }
                        baseSize += 4 * product;
                        break;
                    case WITH_INIT_ID:
                        baseSize += 4;
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
        child = child->rightSibling;
    }
    if (child != NULL) {
        child = child->child;
        for (; child != NULL; child = child->rightSibling) {
            int tmp = 0, tmp2;
            if (child->nodeType == STMT_NODE) {
                switch (STMTKIND(child)) {
                case WHILE_STMT:
                    if (child->child->rightSibling->nodeType == BLOCK_NODE) {
                        tmp = calcBlockOffset(child->child->rightSibling);
                    }
                    break;
                case FOR_STMT:
                    if (child->child->rightSibling->rightSibling->rightSibling->nodeType == BLOCK_NODE) {
                        tmp = calcBlockOffset(child->child->rightSibling->rightSibling->rightSibling);
                    }
                    break;
                case IF_STMT:
                    if (child->child->rightSibling->nodeType == BLOCK_NODE) {
                        tmp = calcBlockOffset(child->child->rightSibling);
                    }
                    if (child->child->rightSibling->rightSibling->nodeType == BLOCK_NODE) {
                        tmp2 = calcBlockOffset(child->child->rightSibling->rightSibling);
                    }
                    if (tmp < tmp2) tmp = tmp2;
                    break;
                default:
                    ;
                }
            } else if (child->nodeType == BLOCK_NODE) {
                tmp = calcBlockOffset(child);
            } else if (child->nodeType == NUL_NODE) {
                // do nothing
            }
            if (innerSize < tmp) innerSize = tmp;
        }
    }
    return baseSize + innerSize;
}

void genFunctionDecl(AST_NODE *funcDeclNode) {
    AST_NODE *typeNode, *idNode, *paramListNode, *blockNode;
    typeNode = funcDeclNode->child;
    idNode = typeNode->rightSibling;
    paramListNode = idNode->rightSibling;
    blockNode = paramListNode->rightSibling;
    curFuncIdNode = idNode;

    emit(".text");
    emit("_start_%s:", IDSTR(idNode));
    emit("str lr, [sp, #0]");
    emit("str fp, [sp, #-4]");
    emit("sub fp, sp, #4");

    int frameSize = calcBlockOffset(blockNode);
    emit("sub sp, fp, #%d", 4 + frameSize + 64);
    emit("str r4, [sp, #4]");
    emit("str r5, [sp, #8]");
    emit("str r6, [sp, #12]");
    emit("str r7, [sp, #16]");
    emit("str r8, [sp, #20]");
    emit("str r9, [sp, #24]");
    emit("str r10, [sp, #28]");
    emit("str r11, [sp, #32]");
    emit("vstr.f32 s16, [sp, #36]");
    emit("vstr.f32 s17, [sp, #40]");
    emit("vstr.f32 s18, [sp, #44]");
    emit("vstr.f32 s19, [sp, #48]");
    emit("vstr.f32 s20, [sp, #52]");
    emit("vstr.f32 s21, [sp, #56]");
    emit("vstr.f32 s22, [sp, #60]");
    emit("vstr.f32 s23, [sp, #64]");

    genBlock(blockNode);

    emit("_end_%s:", IDSTR(idNode));
    emit("ldr r4, [sp, #4]");
    emit("ldr r5, [sp, #8]");
    emit("ldr r6, [sp, #12]");
    emit("ldr r7, [sp, #16]");
    emit("ldr r8, [sp, #20]");
    emit("ldr r9, [sp, #24]");
    emit("ldr r10, [sp, #28]");
    emit("ldr r11, [sp, #32]");
    emit("vldr.f32 s16, [sp, #36]");
    emit("vldr.f32 s17, [sp, #40]");
    emit("vldr.f32 s18, [sp, #44]");
    emit("vldr.f32 s19, [sp, #48]");
    emit("vldr.f32 s20, [sp, #52]");
    emit("vldr.f32 s21, [sp, #56]");
    emit("vldr.f32 s22, [sp, #60]");
    emit("vldr.f32 s23, [sp, #64]");
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
