#include "codegen.h"

const char *REG[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "sp", "lr", "pc",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "s12", "s13", "s14", "s15",
    "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23", "s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31"
};

FILE *outfile;
AST_NODE *curFuncIdNode;
int cntConst;
int cntWhile;
int cntFor;
int cntElse;
int cntIf;
int cntfunc; // make function call easy to read
int cntShortCurcuit;

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

REGISTER allocateRegister(SymbolTableEntry *sym) {
    // TODO
    return R4;
}

void genIntBinOp(BINARY_OPERATOR op, REGISTER dst, REGISTER lhs, REGISTER rhs) {
    switch (op) {
    case BINARY_OP_ADD: emit("add %s, %s, %s", REG[dst], REG[lhs], REG[rhs]); break;
    case BINARY_OP_SUB: emit("sub %s, %s, %s", REG[dst], REG[lhs], REG[rhs]); break;
    case BINARY_OP_MUL: emit("mul %s, %s, %s", REG[dst], REG[lhs], REG[rhs]); break;
    case BINARY_OP_DIV: emit("sdiv %s, %s, %s", REG[dst], REG[lhs], REG[rhs]); break;
    case BINARY_OP_AND: emit("and %s, %s, %s", REG[dst], REG[lhs], REG[rhs]); break;
    case BINARY_OP_OR: emit("orr %s, %s, %s", REG[dst], REG[lhs], REG[rhs]); break;
    case BINARY_OP_EQ: case BINARY_OP_GE: case BINARY_OP_LE:
    case BINARY_OP_NE: case BINARY_OP_GT: case BINARY_OP_LT:
        emit("mov %s, #0", REG[dst]);
        emit("cmp %s, %s", REG[lhs], REG[rhs]);
        switch (op) {
        case BINARY_OP_EQ: emit("moveq %s, #1", REG[dst]); break;
        case BINARY_OP_GE: emit("movge %s, #1", REG[dst]); break;
        case BINARY_OP_LE: emit("movle %s, #1", REG[dst]); break;
        case BINARY_OP_NE: emit("movne %s, #1", REG[dst]); break;
        case BINARY_OP_GT: emit("movgt %s, #1", REG[dst]); break;
        case BINARY_OP_LT: emit("movlt %s, #1", REG[dst]); break;
        }
        break;
    default: ;
    }
}

void genIntUniOp(UNARY_OPERATOR op, REGISTER dst, REGISTER lhs) {
    switch (op) {
    case UNARY_OP_POSITIVE: emit("mov %s, %s", REG[dst], REG[lhs]); break;
    case UNARY_OP_NEGATIVE: emit("neg %s, %s", REG[dst], REG[lhs]); break;
    case UNARY_OP_LOGICAL_NEGATION:
        emit("cmp %s, #0", REG[lhs]);
        emit("moveq %s, #1", REG[dst]);
        emit("movne %s, #0", REG[dst]);
        break;
    default: ;
    }
}

void genFloatBinOp(BINARY_OPERATOR op, REGISTER dst, REGISTER lhs, REGISTER rhs) {
    switch (op) {
    case BINARY_OP_ADD: emit("vadd.f32 %s, %s, %s", REG[dst], REG[lhs], REG[rhs]); break;
    case BINARY_OP_SUB: emit("vsub.f32 %s, %s, %s", REG[dst], REG[lhs], REG[rhs]); break;
    case BINARY_OP_MUL: emit("vmul.f32 %s, %s, %s", REG[dst], REG[lhs], REG[rhs]); break;
    case BINARY_OP_DIV: emit("vdiv.f32 %s, %s, %s", REG[dst], REG[lhs], REG[rhs]); break;
    case BINARY_OP_AND: break;
    case BINARY_OP_OR: break;
    case BINARY_OP_EQ: case BINARY_OP_GE: case BINARY_OP_LE:
    case BINARY_OP_NE: case BINARY_OP_GT: case BINARY_OP_LT:
        emit("mov %s, #0", REG[dst]);
        emit("vcmp.f32 %s, %s", REG[lhs], REG[rhs]);
        emit("vmrs apsr_nzcv, fpscr");
        switch (op) {
        case BINARY_OP_EQ: emit("moveq %s, #1", REG[dst]); break;
        case BINARY_OP_GE: emit("movge %s, #1", REG[dst]); break;
        case BINARY_OP_LE: emit("movle %s, #1", REG[dst]); break;
        case BINARY_OP_NE: emit("movne %s, #1", REG[dst]); break;
        case BINARY_OP_GT: emit("movgt %s, #1", REG[dst]); break;
        case BINARY_OP_LT: emit("movlt %s, #1", REG[dst]); break;
        }
        break;
    default: ;
    }
}

void genFloatUniOp(UNARY_OPERATOR op, REGISTER dst, REGISTER lhs) {
    switch (op) {
    case UNARY_OP_POSITIVE: emit("vmov.f32 %s, %s", REG[dst], REG[lhs]); break;
    case UNARY_OP_NEGATIVE: emit("vneg.f32 %s, %s", REG[dst], REG[lhs]); break;
    case UNARY_OP_LOGICAL_NEGATION:
        emit("vcmp.f32 %s, #0", REG[lhs]);
        emit("vmrs apsr_nzcv, fpscr");
        emit("moveq %s, #1", REG[dst]);
        emit("movne %s, #0", REG[dst]);
        break;
    default: ;
    }
}

REGISTER genExpr(AST_NODE *node) {
    if (node->dataType == FLOAT_TYPE) return genFloatExpr(node);
    return genIntExpr(node);
}

void genArrayPosition(AST_NODE *node) {
    // position will store in r7
    ArrayProperties properties = SYMARRPROP(IDSYM(node));
    AST_NODE *exprNode = node->child;
    int dim = 1;
    emit("mov r4, #0");
    emit("str r4, [sp]");
    emit("sub sp, #4");
    for (; dim <= properties.dimension && exprNode != NULL; dim++, exprNode = exprNode->rightSibling) {
        int arrSize = (dim == properties.dimension ? 4 : properties.sizeInEachDimension[dim]);
        REGISTER res = genIntExpr(exprNode);
        emit("ldr r7, [sp, #4]");
        emit("add r7, r7, %s", REG[res]);
        emit("mov r4, #%d", arrSize);
        emit("mul r7, r7, r4");
        emit("str r7, [sp, #4]");
    }
    emit("add sp, #4");
    if (IDSYM(node)->nestingLevel == 0) {
        emit("ldr r4, =_g_%s", IDSTR(node));
        emit("add r7, r7, r4");
    }
    else {
        emit("add r7, r7, #%d", IDSYM(node)->offset);
        emit("add r7, r7, fp");
    }
}

REGISTER genIntExpr(AST_NODE *node) {
    // XXX stack machine
    switch (node->nodeType) {
    case EXPR_NODE:
        if (EXPRCONSTEVAL(node)) {
            emit(".data");
            emit("__CONST_%d: .word %d", cntConst, EXPRCONSTU(node).iValue);
            emit(".text");
            emit("ldr r4, =__CONST_%d", cntConst);
            emit("ldr r4, [r4]");
            cntConst++;
        } else if (EXPRKIND(node) == BINARY_OPERATION) {
            switch (EXPRBINOP(node)) {
            case BINARY_OP_AND:
                {
                    REGISTER res = genExpr(node->child);
                    emit("cmp %s, #0", REG[res]);
                    emit("beq __short_cuit__%d", cntShortCurcuit);
                    emit("str %s, [sp]", REG[res]);
                    emit("sub sp, #4");
                    res = genExpr(node->child->rightSibling);
                    emit("add sp, #4");
                    emit("ldr r5, [sp]");
                    emit("mov r6, %s", REG[res]);
                    genIntBinOp(EXPRBINOP(node), R4, R5, R6);
                    emit("__short_cuit__%d:", cntShortCurcuit++);
                }
                break;
            case BINARY_OP_OR:
                {
                    REGISTER res = genExpr(node->child);
                    emit("cmp %s, #1", REG[res]);
                    emit("beq __short_cuit__%d", cntShortCurcuit);
                    emit("str %s, [sp]", REG[res]);
                    emit("sub sp, #4");
                    res = genExpr(node->child->rightSibling);
                    emit("add sp, #4");
                    emit("ldr r5, [sp]");
                    emit("mov r6, %s", REG[res]);
                    genIntBinOp(EXPRBINOP(node), R4, R5, R6);
                    emit("__short_cuit__%d:", cntShortCurcuit++);
                }
                break;
            default:
                {
                    REGISTER res = genExpr(node->child);
                    emit("str %s, [sp]", REG[res]);
                    emit("sub sp, #4");
                    res = genExpr(node->child->rightSibling);
                    emit("add sp, #4");
                    emit("ldr r5, [sp]");
                    emit("mov r6, %s", REG[res]);
                    genIntBinOp(EXPRBINOP(node), R4, R5, R6);
                }
            }
        } else {
           REGISTER res = genExpr(node->child);
           emit("mov r5, #%d", res);
           genIntUniOp(EXPRUNIOP(node), R4, R5);
        }
        break;
    case CONST_VALUE_NODE:
        switch (CONSTTYPE(node)) {
        case INTEGERC:
            emit(".data");
            emit("__CONST_%d: .word %d", cntConst, CONSTU(node).intval);
            emit(".text");
            emit("ldr r4, =__CONST_%d", cntConst);
            emit("ldr r4, [r4]");
            cntConst++;
            break;
        case STRINGC:
            {
                char buf[256];
                int i;
                strncpy(buf, CONSTU(node).sc, 256);
                for (i = 1; buf[i] != '"'; i++);
                buf[i] = '\0';
                emit(".data");
                emit("__CONST_%d: .ascii \"%s\\000\"", cntConst, buf + 1);
                emit(".text");
                emit("ldr r4, =__CONST_%d", cntConst);
                cntConst++;
            }
            break;
        case FLOATC:
        default:
            ;
        }
        break;
    case IDENTIFIER_NODE:
        switch (IDKIND(node)) {
        case NORMAL_ID:
            if (IDSYM(node)->nestingLevel == 0) {
                emit("ldr r7, =_g_%s", IDSTR(node));
                emit("ldr r4, [r7]");
            } else {
                emit("ldr r4, [fp, #%d]", IDSYM(node)->offset);
            }
            break;
        case ARRAY_ID:
            genArrayPosition(node);
            emit("ldr r4, [r7]");
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
    return R4;
}

REGISTER genFloatExpr(AST_NODE *node) {
    // XXX stack machine
    switch (node->nodeType) {
    case EXPR_NODE:
        if (EXPRCONSTEVAL(node)) {
            emit(".data");
            emit("__CONST_%d: .float %f", cntConst, EXPRCONSTU(node).fValue);
            emit(".text");
            emit("ldr r4, =__CONST_%d", cntConst);
            emit("vldr.f32 s16, [r4]");
            cntConst++;
        } else if (EXPRKIND(node) == BINARY_OPERATION) {
            REGISTER res = genExpr(node->child);
            if (node->child->dataType == INT_TYPE) {
                emit("vmov s16, %s", REG[res]);
                emit("vcvt.f32.s32 s16, s16");
                res = S16;
            }
            emit("vstr.f32 %s, [sp]", REG[res]);
            emit("sub sp, #4");
            res = genExpr(node->child->rightSibling);
            if (node->child->rightSibling->dataType == INT_TYPE) {
                emit("vmov s16, %s", REG[res]);
                emit("vcvt.f32.s32 s16, s16");
                res = S16;
            }
            emit("add sp, #4");
            emit("vldr.f32 s17, [sp]");
            emit("vmov.f32 s18, %s", REG[res]);
            switch (EXPRBINOP(node)) {
            case BINARY_OP_EQ: case BINARY_OP_GE: case BINARY_OP_LE:
            case BINARY_OP_NE: case BINARY_OP_GT: case BINARY_OP_LT:
                genFloatBinOp(EXPRBINOP(node), R4, S17, S18);
                return R4;
            default:
                genFloatBinOp(EXPRBINOP(node), S16, S17, S18);
            }
        } else {
            REGISTER res = genExpr(node->child);
            emit("vmov.f32 s17, %s", res);
            if (EXPRUNIOP(node) == UNARY_OP_LOGICAL_NEGATION) {
                genFloatUniOp(EXPRUNIOP(node), R4, S17);
                return R4;
            } else {
                genFloatUniOp(EXPRUNIOP(node), S16, S17);
            }
        }
        break;
    case CONST_VALUE_NODE:
        switch (CONSTTYPE(node)) {
        case FLOATC:
            emit(".data");
            emit("__CONST_%d: .float %f", cntConst, CONSTU(node).fval);
            emit(".text");
            emit("ldr r4, =__CONST_%d", cntConst);
            emit("vldr.f32 s16, [r4]");
            cntConst++;
            break;
        case INTEGERC:
        case STRINGC:
        default:
            ;
        }
        break;
    case IDENTIFIER_NODE:
        switch (IDKIND(node)) {
        case NORMAL_ID:
            if (IDSYM(node)->nestingLevel == 0) {
                emit("ldr r7, =_g_%s", IDSTR(node));
                emit("vldr.f32 s16, [r7]");
            } else {
                emit("vldr.f32 s16, [fp, #%d]", IDSYM(node)->offset);
            }
            break;
        case ARRAY_ID:
            genArrayPosition(node);
            emit("vldr.f32 s16, [r7]");
            break;
        default:
            ;
        }
        break;
    case STMT_NODE:  // FUNCTION_CALL_STMT
        genFuncCall(node);
        return S0;
    default:
        ;
    }
    return S16;
}

void genGlobalVarDecl(AST_NODE *varDeclListNode) {
    // TODO declare global array
    emit(".data");

    AST_NODE *declNode = varDeclListNode->child;
    for (; declNode != NULL; declNode = declNode->rightSibling) {
        // get type
        if (DECLKIND(declNode) == VARIABLE_DECL) {
            char *typeMark;
            AST_NODE *typeNode = declNode->child, *idNode = typeNode->rightSibling;
            SymbolTableEntry *idSym = IDSYM(idNode);
            DATA_TYPE type = idSym->attribute->attr.typeDescriptor->properties.dataType;

            for (; idNode != NULL; idNode = idNode->rightSibling) {
                char name[300];
                sprintf(name, "_g_%s", IDSTR(idNode));
                switch (IDKIND(idNode)) {
                case NORMAL_ID:
                    if (type == INT_TYPE) {
                        int value = 0;
                        typeMark = ".word";
                        if (idNode->child != 0) {
                            value = idNode->child->semantic_value.const1->const_u.intval;
                        }
                        emit("%s: %s %d", name, typeMark, value);
                    } else if (type == FLOAT_TYPE) {
                        double value = 0.0;
                        typeMark = ".float";
                        if (idNode->child != 0) {
                            value = idNode->child->semantic_value.const1->const_u.fval;
                        }
                        emit("%s: %s %lf", name, typeMark, value);
                    }
                    break;
                case ARRAY_ID:
                    {
                        // TODO: write a function to count array size
                        int i, product = 4;
                        for (i = 0; i < SYMARRPROP(IDSYM(idNode)).dimension; i++) {
                            product *= SYMARRPROP(IDSYM(idNode)).sizeInEachDimension[i];
                        }
                        emit("%s: .skip %d", name, product);
                    }
                    break;
                case WITH_INIT_ID:
                    {
                        AST_NODE *constNode = idNode->child;
                        if (idNode->child->dataType == FLOAT_TYPE) {
                            float value = constNode->semantic_value.exprSemanticValue.constEvalValue.fValue;
                            emit("%s: .float %f", name, value);
                        } else {
                            int value = constNode->semantic_value.exprSemanticValue.constEvalValue.iValue;
                            emit("%s: .word %d", name, value);
                        }
                    }
                    break;
                default:
                    ;
                }
            }
        }
    }
}

void genFuncCall(AST_NODE *funcCallStmtNode) {
    emit("func_call_%d:", cntfunc++);
    AST_NODE *funcIdNode, *paramListNode, *exprNode;
    funcIdNode = funcCallStmtNode->child;
    paramListNode = funcIdNode->rightSibling;
    // special cases for read/write
    if (strcmp(IDSTR(funcCallStmtNode->child), "write") == 0) {
        REGISTER reg = genExpr(paramListNode->child);
        if (paramListNode->child->dataType == CONST_STRING_TYPE) {
            emit("mov r0, %s", REG[reg]);
            emit("bl _write_str");
        } else if (paramListNode->child->dataType == FLOAT_TYPE) {
            emit("vmov.f32 s0, %s", REG[reg]);
            emit("bl _write_float");
        } else {
            emit("mov r0, %s", REG[reg]);
            emit("bl _write_int");
        }
    } else {
        int paramCnt = 0;
        if (paramListNode->nodeType != NUL_NODE) {
            for (exprNode = paramListNode->child; exprNode != NULL; exprNode = exprNode->rightSibling) {
                paramCnt++;
            }
            emit("sub sp, sp, #%d", 4 * paramCnt);
            int i = 0;
            Parameter *param = SYMFUNCSIGN(IDSYM(funcIdNode))->parameterList;
            for (exprNode = paramListNode->child; exprNode != NULL; exprNode = exprNode->rightSibling) {
                REGISTER res = genExpr(exprNode);
                if (exprNode->dataType == FLOAT_TYPE) {
                    if (param->type->properties.dataType == FLOAT_TYPE) {
                        emit("vcvt.s32.f32 %s, %s", REG[res], REG[res]);
                        emit("vmov r4, %s", REG[res]);
                        emit("str r4, [sp, #%d]", (++i) * 4);
                    } else {
                        emit("vstr.f32 %s, [sp, #%d]", REG[res], (++i) * 4);
                    }
                } else {
                    if (param->type->properties.dataType == FLOAT_TYPE) {
                        emit("vmov s16, %s", REG[res]);
                        emit("vcvt.f32.s32 s16, s16");
                        emit("vstr.f32 s16, [sp, #%d]", (++i) * 4);
                    } else {
                        emit("str %s, [sp, #%d]", REG[res], (++i) * 4);
                    }
                }
                param = param->next;
            }
        }
        emit("bl _start_%s", IDSTR(funcCallStmtNode->child));
        if (paramCnt > 0) emit("add sp, sp, #%d", 4 * paramCnt);
    }
}

void genAssign(AST_NODE *stmtNode) {
    REGISTER result = genExpr(stmtNode->child->rightSibling);
    switch (IDKIND(stmtNode->child)) {
    case NORMAL_ID:
        if (stmtNode->child->dataType == FLOAT_TYPE) {
            if (stmtNode->child->rightSibling->dataType == INT_TYPE) {
                emit("vmov s16, %s", REG[result]);
                emit("vcvt.f32.s32 s16, s16");
                result = S16;
            }
            if (IDSYM(stmtNode->child)->nestingLevel == 0) {
                emit("ldr r7, =_g_%s", IDSTR(stmtNode->child));
                emit("vstr.f32 %s, [r7]", REG[result]);
            } else {
                emit("vstr.f32 %s, [fp, #%d]", REG[result], IDSYM(stmtNode->child)->offset);
            }
            if (result != S16) emit("vmov.f32 s16, %s", REG[result]);
        } else {
            if (stmtNode->child->rightSibling->dataType == FLOAT_TYPE) {
                emit("vcvt.s32.f32 %s, %s", REG[result], REG[result]);
                emit("vmov r4, %s", REG[result]);
                result = R4;
            }
            if (IDSYM(stmtNode->child)->nestingLevel == 0) {
                emit("ldr r7, =_g_%s", IDSTR(stmtNode->child));
                emit("str %s, [r7]", REG[result]);
            } else {
                emit("str %s, [fp, #%d]", REG[result], IDSYM(stmtNode->child)->offset);
            }
            if (result != R4) emit("mov r4, %s", REG[result]);
        }
        break;
    case ARRAY_ID:
        if (stmtNode->child->dataType == FLOAT_TYPE) {
            if (stmtNode->child->rightSibling->dataType == INT_TYPE) {
                emit("vmov s16, %s", REG[result]);
                emit("vcvt.f32.s32 s16, s16");
                result = S16;
            }
            emit("vstr.f32 %s, [sp]", REG[result]);
            emit("sub sp, #4");
            genArrayPosition(stmtNode->child);
            emit("add sp, #4");
            emit("vldr.f32 %s, [sp]", REG[result]);
            emit("vstr.f32 %s, [r7]", REG[result]);
        } else {
            if (stmtNode->child->rightSibling->dataType == FLOAT_TYPE) {
                emit("vcvt.s32.f32 %s, %s", REG[result], REG[result]);
                emit("vmov r4, %s", REG[result]);
                result = R4;
            }
            emit("str %s, [sp]", REG[result]);
            emit("sub sp, #4");
            genArrayPosition(stmtNode->child);
            emit("add sp, #4");
            emit("ldr %s, [sp]", REG[result]);
            emit("str %s, [r7]", REG[result]);
        }
        break;
    default:
        ;
    }
}

void genIf(AST_NODE *stmtNode) {
    REGISTER result;
    int idIf = cntIf++;
    int idElse = cntElse++;
    AST_NODE *elseNode = stmtNode->child->rightSibling->rightSibling;
    if (stmtNode->child->nodeType == STMT_NODE && STMTKIND(stmtNode->child) == ASSIGN_STMT) {
        genAssign(stmtNode->child);
        result = (stmtNode->child->dataType == FLOAT_TYPE) ? S16 : R4;
    } else {
        result = genExpr(stmtNode->child);
    }
    if (S0 <= result && result <= S31) {
        emit("vcmp.f32 %s, #0.0", REG[result]);
        emit("vmrs apsr_nzcv, fpscr");
        emit("beq __if_end_%d", idIf);
    } else {
        emit("cmp %s, #0", REG[result]);
        emit("beq __if_end_%d", idIf);
    }
    if (stmtNode->child->rightSibling->nodeType == STMT_NODE) {
        genStmt(stmtNode->child->rightSibling);
    } else if (stmtNode->child->rightSibling->nodeType == BLOCK_NODE) {
        genBlock(stmtNode->child->rightSibling);
    }
    emit("b __else_end_%d", idElse);
    emit("__if_end_%d:", idIf);
    if (elseNode->nodeType != NUL_NODE) {
        if (elseNode->nodeType == STMT_NODE) {
            genStmt(elseNode);
        } else if (elseNode->nodeType == BLOCK_NODE) {
            genBlock(elseNode);
        }
    }
    emit("__else_end_%d:", idElse);
}

void genStmt(AST_NODE *stmtNode) {
    switch (STMTKIND(stmtNode)) {
    case WHILE_STMT:
        {
            int idWhile = cntWhile++;
            REGISTER result;
            emit("__while_start_%d:", idWhile);
            if (stmtNode->child->nodeType == STMT_NODE && STMTKIND(stmtNode->child) == ASSIGN_STMT) {
                genAssign(stmtNode->child);
                result = (stmtNode->child->dataType == FLOAT_TYPE) ? S16 : R4;
            } else {
                result = genExpr(stmtNode->child);
            }
            if (S0 <= result && result <= S31) {
                emit("vcmp.f32 %s, #0.0", REG[result]);
                emit("vmrs apsr_nzcv, fpscr");
                emit("beq __while_end_%d", idWhile);
            } else {
                emit("cmp %s, #0", REG[result]);
                emit("beq __while_end_%d", idWhile);
            }
            if (stmtNode->child->rightSibling->nodeType == STMT_NODE) {
                genStmt(stmtNode->child->rightSibling);
            } else if (stmtNode->child->rightSibling->nodeType == BLOCK_NODE) {
                genBlock(stmtNode->child->rightSibling);
            } else if (stmtNode->child->rightSibling->nodeType == NUL_NODE) {
                // do nothing
            }
            emit("b __while_start_%d", idWhile);
            emit("__while_end_%d:", idWhile);
        }
        break;
    case FOR_STMT:
        {
            int idFor = cntFor++;
            REGISTER result;
            AST_NODE *initNode, *condNode, *incrNode, *loopNode;
            initNode = stmtNode->child;
            condNode = initNode->rightSibling;
            incrNode = condNode->rightSibling;
            loopNode = incrNode->rightSibling;
            emit("__for_start_%d:", idFor);
            if (initNode->nodeType == NONEMPTY_ASSIGN_EXPR_LIST_NODE) {
                AST_NODE *assignNode = initNode->child;
                for (; assignNode != NULL; assignNode = assignNode->rightSibling) {
                    genAssign(assignNode);
                }
            }
            emit("__for_loop_%d:", idFor);
            if (condNode->nodeType == NONEMPTY_RELOP_EXPR_LIST_NODE) {
                AST_NODE *exprNode = condNode->child;
                for (; exprNode != NULL; exprNode = exprNode->rightSibling) {
                    result = genExpr(exprNode);
                }
                if (S0 <= result && result <= S31) {
                    emit("vcmp.f32 %s, #0.0", REG[result]);
                    emit("vmrs apsr_nzcv, fpscr");
                    emit("beq __for_end_%d", idFor);
                } else {
                    emit("cmp %s, #0", REG[result]);
                    emit("beq __for_end_%d", idFor);
                }
            }
            if (loopNode->nodeType == STMT_NODE) {
                genStmt(loopNode);
            } else if (loopNode->nodeType == BLOCK_NODE) {
                genBlock(loopNode);
            } else if (loopNode->nodeType == NUL_NODE) {
                // do nothing
            }
            if (incrNode->nodeType == NONEMPTY_ASSIGN_EXPR_LIST_NODE) {
                AST_NODE *assignNode = incrNode->child;
                for (; assignNode != NULL; assignNode = assignNode->rightSibling) {
                    if (assignNode->nodeType == STMT_NODE) {
                        genAssign(assignNode);
                    } else if (assignNode->nodeType == EXPR_NODE) {
                        genExpr(assignNode);
                    }
                }
            }
            emit("b __for_loop_%d", idFor);
            emit("__for_end_%d:", idFor);
        }
        break;
    case ASSIGN_STMT:
        genAssign(stmtNode);
        break;
    case IF_STMT:
        genIf(stmtNode);
        break;
    case FUNCTION_CALL_STMT:
        genFuncCall(stmtNode);
        break;
    case RETURN_STMT:
        if (stmtNode->child->nodeType != NUL_NODE) {
            REGISTER result = genExpr(stmtNode->child);
            if (stmtNode->child->dataType == FLOAT_TYPE) {
                if (result != S0) emit("vmov.f32 s0, %s", REG[result]);
            } else {
                if (result != R0) emit("mov r0, %s", REG[result]);
            }
        }
        emit("b _end_%s", IDSTR(curFuncIdNode));
        break;
    default:
        ;
    }
}

void genStmtList(AST_NODE *stmtListNode) {
    AST_NODE *child = stmtListNode->child;
    for (; child != NULL; child = child->rightSibling) {
        if (child->nodeType == STMT_NODE) {
            genStmt(child);
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
                        REGISTER result = genExpr(idNode->child);
                        if (idNode->child->dataType == FLOAT_TYPE) {
                            if (SYMTYPEDESC(IDSYM(idNode))->properties.dataType == INT_TYPE) {
                                emit("vcvt.s32.f32 %s, %s", REG[result], REG[result]);
                                emit("vmov r4, %s", REG[result]);
                                emit("str r4, [fp, #%d]", IDSYM(idNode)->offset);
                            } else {
                                emit("vstr.f32 %s, [fp, #%d]", REG[result], IDSYM(idNode)->offset);
                            }
                        } else {
                            if (SYMTYPEDESC(IDSYM(idNode))->properties.dataType == FLOAT_TYPE) {
                                emit("vmov s16, %s", REG[result]);
                                emit("vcvt.f32.s32 s16, s16");
                                emit("vstr.f32 s16, [fp, #%d]", IDSYM(idNode)->offset);
                            } else {
                                emit("str %s, [fp, #%d]", REG[result], IDSYM(idNode)->offset);
                            }
                        }
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
    emit("and sp, sp, #0xfffffff0");  // stack align
    emit("bl __callee_reg_save");

    genBlock(blockNode);

    emit("_end_%s:", IDSTR(idNode));
    emit("bl __callee_reg_restore");
    emit("ldr lr, [fp, #4]");
    emit("add sp, fp, #4");
    emit("ldr fp, [fp, #0]");
    emit("bx lr");
}

void genCalleeSaveRestore() {
    emit("__callee_reg_save:");
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
    emit("bx lr");
    emit("__callee_reg_restore:");
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
    emit("bx lr");
}

void genReadStub() {
    emit("_start_read:");
    emit("b _read_int");
    emit("_start_fread:");
    emit("b _read_float");
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

    genCalleeSaveRestore();
    genReadStub();
    codegenEnd();
}
