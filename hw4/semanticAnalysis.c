#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "symbolTable.h"
// This file is for reference only, you are not required to follow the implementation. //
// You only need to check for errors stated in the hw4 document. //
int g_anyErrorOccur = 0;

DATA_TYPE getBiggerType(DATA_TYPE dataType1, DATA_TYPE dataType2);
void visit(AST_NODE *node);
void visitChildren(AST_NODE *node);
void processProgramNode(AST_NODE *programNode);
void processDeclarationNode(AST_NODE* declarationNode);
void declareIdList(AST_NODE* typeNode, SymbolAttributeKind isVariableOrTypeAttribute, int ignoreArrayFirstDimSize);
void declareFunction(AST_NODE* returnTypeNode);
void processDeclDimList(AST_NODE* varDeclDimListNode, TypeDescriptor* typeDescriptor, int ignoreFirstDimSize);
void processTypeNode(AST_NODE* typeNode);
void processIdentifierNode(AST_NODE* identifierNode);
void processParamListNode(AST_NODE* paramListNode);
void processBlockNode(AST_NODE* blockNode);
void processVarDeclListNode(AST_NODE* varDeclListNode);
void processStmtListNode(AST_NODE* stmtListNode);
void processStmtNode(AST_NODE* stmtNode);
void processGeneralNode(AST_NODE *node);
void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode);
void checkWhileStmt(AST_NODE* whileNode);
void checkForStmt(AST_NODE* forNode);
void checkAssignmentStmt(AST_NODE* assignmentNode);
void checkIfStmt(AST_NODE* ifNode);
void checkWriteFunction(AST_NODE* functionCallNode);
void checkFunctionCall(AST_NODE* functionCallNode);
void processExprRelatedNode(AST_NODE* exprRelatedNode);
void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter);
void checkReturnStmt(AST_NODE* returnNode);
void processExprNode(AST_NODE* exprNode);
void processVariableLValue(AST_NODE* idNode);
void processVariableRValue(AST_NODE* idNode);
void processConstValueNode(AST_NODE* constValueNode);
void processNonemptyAssignExprListNode(AST_NODE* node);
void processNonemptyRelOpExprListNode(AST_NODE* node);
void getExprOrConstValue(AST_NODE* exprOrConstNode, int* iValue, float* fValue);
void evaluateExprValue(AST_NODE* exprNode);


typedef enum ErrorMsgKind
{
    SYMBOL_IS_NOT_TYPE,
    SYMBOL_REDECLARE,
    SYMBOL_UNDECLARED,
    NOT_FUNCTION_NAME,
    TRY_TO_INIT_ARRAY,
    EXCESSIVE_ARRAY_DIM_DECLARATION,
    RETURN_ARRAY,
    VOID_VARIABLE,
    TYPEDEF_VOID_ARRAY,
    PARAMETER_TYPE_UNMATCH,
    TOO_FEW_ARGUMENTS,
    TOO_MANY_ARGUMENTS,
    RETURN_TYPE_UNMATCH,
    INCOMPATIBLE_ARRAY_DIMENSION,
    NOT_ASSIGNABLE,
    NOT_ARRAY,
    IS_TYPE_NOT_VARIABLE,
    IS_FUNCTION_NOT_VARIABLE,
    STRING_OPERATION,
    ARRAY_SIZE_NOT_INT,
    ARRAY_SIZE_NEGATIVE,
    ARRAY_SUBSCRIPT_NOT_INT,
    PASS_ARRAY_TO_SCALAR,
    PASS_SCALAR_TO_ARRAY
} ErrorMsgKind;

void printErrorMsgSpecial(AST_NODE* node1, char* name2, ErrorMsgKind errorMsgKind)
{
    g_anyErrorOccur = 1;
    printf("Error found in line %d\n", node1->linenumber);
    /*
    switch(errorMsgKind)
    {
    default:
        printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
        break;
    }
    */
}


void printErrorMsg(AST_NODE* node, ErrorMsgKind errorMsgKind)
{
    g_anyErrorOccur = 1;
    printf("Error found in line %d\n", node->linenumber);
    /*
    switch(errorMsgKind)
    {
        printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
        break;
    }
    */
}


void visit(AST_NODE *node)
{
    switch (node->nodeType) {
        case PROGRAM_NODE:
            processProgramNode(node);
            break;
        case DECLARATION_NODE:
            processDeclarationNode(node);
            break;
        case IDENTIFIER_NODE:
            processIdentifierNode(node);
            break;
        case PARAM_LIST_NODE:
            processParamListNode(node);
            break;
        case NUL_NODE:
            break;
        case BLOCK_NODE:
            processBlockNode(node);
            break;
        case VARIABLE_DECL_LIST_NODE:
            processVarDeclListNode(node);
            break;
        case STMT_LIST_NODE:
            processStmtListNode(node);
            break;
        case STMT_NODE:
            processStmtNode(node);
            break;
        case EXPR_NODE:
            processExprNode(node);
            break;
        case CONST_VALUE_NODE:
            processConstValueNode(node);
            break;
        case NONEMPTY_ASSIGN_EXPR_LIST_NODE:
            processNonemptyAssignExprListNode(node);
            break;
        case NONEMPTY_RELOP_EXPR_LIST_NODE:
            processNonemptyRelOpExprListNode(node);
            break;
        default:
            ;
    }
}


void visitChildren(AST_NODE *node)
{
    AST_NODE *child;
    for (child = node->child; child != NULL; child = child->rightSibling) {
        visit(child);
    }
}


void semanticAnalysis(AST_NODE *root)
{
    processProgramNode(root);
}


DATA_TYPE getBiggerType(DATA_TYPE dataType1, DATA_TYPE dataType2)
{
    if(dataType1 == FLOAT_TYPE || dataType2 == FLOAT_TYPE) {
        return FLOAT_TYPE;
    } else {
        return INT_TYPE;
    }
}


void processProgramNode(AST_NODE *programNode)
{
    visitChildren(programNode);
}

void processDeclarationNode(AST_NODE* declarationNode)
{
    switch (declarationNode->semantic_value.declSemanticValue.kind) {
        case VARIABLE_DECL:
            declareIdList(declarationNode->child, VARIABLE_ATTRIBUTE, 0);
            break;
        case TYPE_DECL:
            declareIdList(declarationNode->child, TYPE_ATTRIBUTE, 0);
            break;
        case FUNCTION_DECL:
            declareFunction(declarationNode->child);
            break;
        case FUNCTION_PARAMETER_DECL:
            declareIdList(declarationNode->child, VARIABLE_ATTRIBUTE, 1);
            break;
        default:
            ;
    }
}


void processTypeNode(AST_NODE* idNodeAsType)
{
}


void declareIdList(AST_NODE* typeNode, SymbolAttributeKind isVariableOrTypeAttribute, int ignoreArrayFirstDimSize)
{
    // children: IDENTIFIER_NODE (type), IDENTIFIER_NODE (id) * n
    AST_NODE *idNode = typeNode->rightSibling;
    switch (isVariableOrTypeAttribute) {
        case VARIABLE_ATTRIBUTE:
            for (; idNode != NULL; idNode = idNode->rightSibling) {
                // TODO
            }
            break;
        case TYPE_ATTRIBUTE:
            for (; idNode != NULL; idNode = idNode->rightSibling) {
                // TODO
            }
            break;
        default:
            ;
    }
}

void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode)
{
}

void checkWhileStmt(AST_NODE* whileNode)
{
}


void checkForStmt(AST_NODE* forNode)
{
}


void checkAssignmentStmt(AST_NODE* assignmentNode)
{
}


void checkIfStmt(AST_NODE* ifNode)
{
}

void checkWriteFunction(AST_NODE* functionCallNode)
{
}

void checkFunctionCall(AST_NODE* functionCallNode)
{
}

void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter)
{
}


void processExprRelatedNode(AST_NODE* exprRelatedNode)
{
}

void getExprOrConstValue(AST_NODE* exprOrConstNode, int* iValue, float* fValue)
{
}

void evaluateExprValue(AST_NODE* exprNode)
{
}


void processExprNode(AST_NODE* exprNode)
{
}


void processVariableLValue(AST_NODE* idNode)
{
}

void processVariableRValue(AST_NODE* idNode)
{
}


void processConstValueNode(AST_NODE* constValueNode)
{
}


void checkReturnStmt(AST_NODE* returnNode)
{
}


void processBlockNode(AST_NODE* blockNode)
{
    openScope();
    visitChildren(blockNode);
    closeScope();
}


void processStmtNode(AST_NODE* stmtNode)
{
    switch (stmtNode->semantic_value.stmtSemanticValue.kind) {
        case WHILE_STMT:
            checkWhileStmt(stmtNode);
            break;
        case FOR_STMT:
            checkForStmt(stmtNode);
            break;
        case ASSIGN_STMT:
            checkAssignmentStmt(stmtNode);
            break;
        case IF_STMT:
            checkIfStmt(stmtNode);
            break;
        case FUNCTION_CALL_STMT:
            checkFunctionCall(stmtNode);
            break;
        case RETURN_STMT:
            checkReturnStmt(stmtNode);
            break;
        default:
            ;
    }
}


void processGeneralNode(AST_NODE *node)
{
}

void processDeclDimList(AST_NODE* varDeclDimListNode, TypeDescriptor* typeDescriptor, int ignoreFirstDimSize)
{
}


void declareFunction(AST_NODE* returnTypeNode)
{
    AST_NODE *idNode, *paramListNode, *blockNode, *paramNode;
    idNode = returnTypeNode->rightSibling;
    paramListNode = idNode->rightSibling;
    blockNode = paramListNode->rightSibling;

    // TODO
    openScope();
    visitChildren(paramListNode);
    visitChildren(blockNode);
    closeScope();
}


void processIdentifierNode(AST_NODE* identifierNode)
{
    const char *TYPES[] = {"void", "int", "float", NULL};
    char *id = identifierNode->semantic_value.identifierSemanticValue.identifierName;
    TypeDescriptor *typeDescriptor;
    int i;
    for (i = 0; TYPES[i] != NULL; i++) {
        if (strcmp(id, TYPES[i]) == 0) {
            processTypeNode(identifierNode);
            return;
        }
    }

    switch (identifierNode->semantic_value.identifierSemanticValue.kind) {
        case NORMAL_ID:
            // TODO
            break;
        case ARRAY_ID:
            typeDescriptor = malloc(sizeof(TypeDescriptor));
            processDeclDimList(identifierNode, typeDescriptor, 0);
            // TODO
            break;
        case WITH_INIT_ID:
            // TODO
            break;
        default:
            ;
    }
}


void processParamListNode(AST_NODE* paramListNode)
{
}


void processVarDeclListNode(AST_NODE* varDeclListNode)
{
}


void processStmtListNode(AST_NODE* stmtListNode)
{
}


void processNonemptyAssignExprListNode(AST_NODE* node)
{
}


void processNonemptyRelOpExprListNode(AST_NODE* node)
{
}
