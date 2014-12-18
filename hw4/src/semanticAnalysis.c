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
void processIdentifierNode(AST_NODE* identifierNode, int matchDimensions);
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

int checkRedeclared(AST_NODE *idNode, int checkLocal);
char *getIdName(AST_NODE *idNode);
SymbolTableEntry *getIdSymtabEntry(AST_NODE *idNode);
void setIdSymtabEntry(AST_NODE *idNode, SymbolTableEntry *sym);
IDENTIFIER_KIND getIdKind(AST_NODE *idNode);
void setIdKind(AST_NODE *idNode, IDENTIFIER_KIND kind);
void checkArrayDereference(AST_NODE *idNode, int matchDimensions);
TypeDescriptor *getTypeDescriptor(SymbolTableEntry *sym);

DATA_TYPE currentFunctionReturnType;


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
    switch(errorMsgKind)
    {
    case PASS_ARRAY_TO_SCALAR:
        printf("Array %s passed to scalar parameter %s.\n", getIdName(node1), name2);
        break;
    case PASS_SCALAR_TO_ARRAY:
        printf("Scalar %s passed to array parameter %s.\n", getIdName(node1), name2);
        break;
    default:
        printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
        break;
    }
}


void printErrorMsg(AST_NODE* node, ErrorMsgKind errorMsgKind)
{
    g_anyErrorOccur = 1;
    printf("Error found in line %d\n", node->linenumber);
    switch(errorMsgKind)
    {
        case SYMBOL_IS_NOT_TYPE:
        printf("ID %s is not a type.\n", getIdName(node));
        break;
    case SYMBOL_REDECLARE:
        printf("ID %s redeclared.\n", getIdName(node));
        break;
    case SYMBOL_UNDECLARED:
        printf("ID %s undeclared.\n", getIdName(node));
        break;
    case NOT_FUNCTION_NAME:
        printf("ID %s is not a function.\n", getIdName(node));
        break;
    case RETURN_ARRAY:
        printf("Function returns array.\n");
        break;
    case TOO_FEW_ARGUMENTS:
        printf("Too few arguments to function %s.\n", getIdName(node));
        break;
    case TOO_MANY_ARGUMENTS:
        printf("Too many arguments to function %s.\n", getIdName(node));
        break;
    case RETURN_TYPE_UNMATCH:
        printf("Incompatible return type.\n");
        break;
    case INCOMPATIBLE_ARRAY_DIMENSION:
        printf("Incompatible array dimensions for array ID %s.\n", getIdName(node));
        break;
    case IS_TYPE_NOT_VARIABLE:
        printf("ID %s is type, not variable.\n", getIdName(node));
        break;
    case IS_FUNCTION_NOT_VARIABLE:
        printf("ID %s is function, not variable.\n", getIdName(node));
        break;
    case ARRAY_SIZE_NOT_INT:
        printf("Array dimensions of ID %s is not integer.\n", getIdName(node));
        break;
    case ARRAY_SIZE_NEGATIVE:
        printf("Array dimensions of ID %s is negative.\n", getIdName(node));
        break;
    case ARRAY_SUBSCRIPT_NOT_INT:
        printf("Array subscript of ID %s is not integer.\n", getIdName(node));
        break;
    default:
        printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
        break;
    }
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
            processIdentifierNode(node, 1);
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
    char *typeName = getIdName(idNodeAsType);
    SymbolTableEntry *sym = retrieveSymbol(typeName);
    if (sym == NULL) {
        printErrorMsg(idNodeAsType, SYMBOL_UNDECLARED);
        // set to void to prevent segfault
        setIdSymtabEntry(idNodeAsType, retrieveSymbol("void"));
    } else {
        if (sym->attribute->attributeKind != TYPE_ATTRIBUTE) {
            printErrorMsg(idNodeAsType, SYMBOL_IS_NOT_TYPE);
        } else {
            setIdSymtabEntry(idNodeAsType, sym);
        }
    }
}


void declareIdList(AST_NODE* typeNode, SymbolAttributeKind isVariableOrTypeAttribute, int ignoreArrayFirstDimSize)
{
    // children: IDENTIFIER_NODE (type), IDENTIFIER_NODE (id) * n
    AST_NODE *idNode = typeNode->rightSibling;
    processTypeNode(typeNode);
    DATA_TYPE dataType = getTypeDescriptor(getIdSymtabEntry(typeNode))->properties.dataType;

    for (; idNode != NULL; idNode = idNode->rightSibling) {
        char *id = getIdName(idNode);
        TypeDescriptor *typeDesc = malloc(sizeof(TypeDescriptor));
        if (getIdKind(idNode) == ARRAY_ID) {
            processDeclDimList(idNode->child, typeDesc, ignoreArrayFirstDimSize);
            typeDesc->properties.arrayProperties.elementType = dataType;
            // TODO: check TRY_TO_INIT_ARRAY
        } else {
            typeDesc->kind = SCALAR_TYPE_DESCRIPTOR;
            typeDesc->properties.dataType = dataType;

            if (getIdKind(idNode) == WITH_INIT_ID) {
                visit(idNode->child);
            }
        }

        SymbolAttribute *symAttr = malloc(sizeof(SymbolAttribute));
        symAttr->attributeKind = isVariableOrTypeAttribute;
        symAttr->attr.typeDescriptor = typeDesc;

        if (checkRedeclared(idNode, 1)) {
            setIdSymtabEntry(idNode, enterSymbol(id, symAttr));
        } else {
            // ignore, use predefined symbol
            setIdSymtabEntry(idNode, retrieveSymbol(id));
        }
    }
}

void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode)
{
}

void checkWhileStmt(AST_NODE* whileNode)
{
    visitChildren(whileNode);
}


void checkForStmt(AST_NODE* forNode)
{
    visitChildren(forNode);
}


void checkAssignmentStmt(AST_NODE* assignmentNode)
{
    visitChildren(assignmentNode);
    AST_NODE *lhs, *rhs;
    lhs = assignmentNode->child;
    rhs = lhs->rightSibling;
}


void checkIfStmt(AST_NODE* ifNode)
{
    visitChildren(ifNode);
}

void checkWriteFunction(AST_NODE* functionCallNode)
{
}

void checkFunctionCall(AST_NODE* functionCallNode)
{
    AST_NODE *idNode, *paramListNode;
    idNode = functionCallNode->child;
    paramListNode = idNode->rightSibling;

    SymbolTableEntry *sym = retrieveSymbol(getIdName(idNode));
    setIdSymtabEntry(idNode, sym);
    if (sym == NULL) {
        printErrorMsg(idNode, SYMBOL_UNDECLARED);
        functionCallNode->dataType = VOID_TYPE;
    } else if (sym->attribute->attributeKind != FUNCTION_SIGNATURE) {
        printErrorMsg(idNode, NOT_FUNCTION_NAME);
        functionCallNode->dataType = VOID_TYPE;
    } else {
        checkParameterPassing(sym->attribute->attr.functionSignature->parameterList, paramListNode);
        functionCallNode->dataType = sym->attribute->attr.functionSignature->returnType;
    }
}

void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter)
{
    AST_NODE *paramNode = actualParameter->child;
    while (paramNode != NULL && formalParameter != NULL) {
        if (paramNode->nodeType == IDENTIFIER_NODE) {
            processIdentifierNode(paramNode, 0);
        } else {
            visit(paramNode);
        }
        switch (formalParameter->type->kind) {
            case SCALAR_TYPE_DESCRIPTOR:
                if (paramNode->nodeType == IDENTIFIER_NODE && getIdKind(paramNode) == ARRAY_ID) {
                    printErrorMsgSpecial(paramNode, formalParameter->parameterName, PASS_ARRAY_TO_SCALAR);
                }
                break;
            case ARRAY_TYPE_DESCRIPTOR:
                if (!(paramNode->nodeType == IDENTIFIER_NODE && getIdKind(paramNode) == ARRAY_ID)) {
                    // XXX: pass expr to array?
                    printErrorMsgSpecial(paramNode, formalParameter->parameterName, PASS_SCALAR_TO_ARRAY);
                }
                break;
            default:
                ;
        }
        // TODO: check dimension match

        paramNode = paramNode->rightSibling;
        formalParameter = formalParameter->next;
    }
    if (paramNode != NULL) {
        printErrorMsg(actualParameter->leftmostSibling, TOO_MANY_ARGUMENTS);
    }
    if (formalParameter != NULL) {
        printErrorMsg(actualParameter->leftmostSibling, TOO_FEW_ARGUMENTS);
    }
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
    visitChildren(exprNode);
    AST_NODE *arg1, *arg2;
    arg1 = exprNode->child;
    arg2 = arg1->rightSibling;
    switch (exprNode->semantic_value.exprSemanticValue.kind) {
        case BINARY_OPERATION:
            exprNode->dataType = getBiggerType(arg1->dataType, arg2->dataType);
            break;
        case UNARY_OPERATION:
            exprNode->dataType = arg1->dataType;
            break;
        default:
            ;
    }
}


void processVariableLValue(AST_NODE* idNode)
{
}

void processVariableRValue(AST_NODE* idNode)
{
}


void processConstValueNode(AST_NODE* constValueNode)
{
    switch (constValueNode->semantic_value.const1->const_type) {
        case INTEGERC:
            constValueNode->dataType = INT_TYPE;
            break;
        case FLOATC:
            constValueNode->dataType = FLOAT_TYPE;
            break;
        default:
            ;
    }
}


void checkReturnStmt(AST_NODE* returnNode)
{
    AST_NODE *returnVal = returnNode->child;
    visit(returnVal);
    if (returnVal->dataType != currentFunctionReturnType) {
        printErrorMsg(returnNode, RETURN_TYPE_UNMATCH);
    } else if (returnVal->nodeType == IDENTIFIER_NODE && returnVal->semantic_value.identifierSemanticValue.kind == ARRAY_ID) {
        printErrorMsg(returnNode, RETURN_ARRAY);
    }
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
    typeDescriptor->kind = ARRAY_TYPE_DESCRIPTOR;
    int dim = 0;
    for (; varDeclDimListNode != NULL; varDeclDimListNode = varDeclDimListNode->rightSibling) {
        if (dim == 0 && ignoreFirstDimSize) {
            typeDescriptor->properties.arrayProperties.sizeInEachDimension[dim] = -1;
        } else {
            if (varDeclDimListNode->nodeType != CONST_VALUE_NODE || varDeclDimListNode->semantic_value.const1->const_type != INTEGERC) {
                printErrorMsg(varDeclDimListNode, ARRAY_SIZE_NOT_INT);
            } else {
                int size = varDeclDimListNode->semantic_value.const1->const_u.intval;
                if (size < 0) {
                    printErrorMsg(varDeclDimListNode, ARRAY_SIZE_NEGATIVE);
                } else {
                    typeDescriptor->properties.arrayProperties.sizeInEachDimension[dim] = size;
                }
            }
        }
        dim++;
    }
    typeDescriptor->properties.arrayProperties.dimension = dim;
}


void declareFunction(AST_NODE* returnTypeNode)
{
    AST_NODE *idNode, *paramListNode, *blockNode;
    idNode = returnTypeNode->rightSibling;
    paramListNode = idNode->rightSibling;
    blockNode = paramListNode->rightSibling;
    char *id = getIdName(idNode);

    processTypeNode(returnTypeNode);

    SymbolAttribute *symAttr = malloc(sizeof(SymbolAttribute));
    symAttr->attributeKind = FUNCTION_SIGNATURE;

    if (checkRedeclared(idNode, 0)) {
        setIdSymtabEntry(idNode, enterSymbol(id, symAttr));
    } else {
        // ignore, use predefined symbol
        setIdSymtabEntry(idNode, retrieveSymbol(id));
    }

    openScope();
    int numParams = 0;
    AST_NODE *paramDeclNode = paramListNode->child;
    Parameter *paramList = NULL;
    for (; paramDeclNode != NULL; paramDeclNode = paramDeclNode->rightSibling) {
        visit(paramDeclNode);
        AST_NODE *idNode = paramDeclNode->child->rightSibling;
        Parameter *newParam = malloc(sizeof(Parameter));
        newParam->type = getTypeDescriptor(getIdSymtabEntry(idNode));
        newParam->parameterName = getIdName(idNode);
        // XXX: O(n^2) insert at bottom
        newParam->next = NULL;
        if (paramList == NULL) paramList = newParam;
        else {
            Parameter *cur = paramList;
            while (cur->next != NULL) cur = cur->next;
            cur->next = newParam;
        }
        numParams++;
    }

    symAttr->attr.functionSignature = malloc(sizeof(FunctionSignature));
    symAttr->attr.functionSignature->parametersCount = numParams;
    symAttr->attr.functionSignature->parameterList = paramList;
    symAttr->attr.functionSignature->returnType = getTypeDescriptor(getIdSymtabEntry(returnTypeNode))->properties.dataType;

    currentFunctionReturnType = getTypeDescriptor(getIdSymtabEntry(returnTypeNode))->properties.dataType;
    visitChildren(blockNode);
    closeScope();
}


void processIdentifierNode(AST_NODE* identifierNode, int matchDimensions)
{
    SymbolTableEntry *sym = retrieveSymbol(getIdName(identifierNode));
    setIdSymtabEntry(identifierNode, sym);
    if (sym == NULL) {
        printErrorMsg(identifierNode, SYMBOL_UNDECLARED);
        // prevent segfault
        setIdSymtabEntry(identifierNode, retrieveSymbol("void"));
    } else if (sym->attribute->attributeKind == TYPE_ATTRIBUTE) {
        printErrorMsg(identifierNode, IS_TYPE_NOT_VARIABLE);
    } else if (sym->attribute->attributeKind == FUNCTION_SIGNATURE) {
        printErrorMsg(identifierNode, IS_FUNCTION_NOT_VARIABLE);
    } else {
        switch (getTypeDescriptor(sym)->kind) {
            case SCALAR_TYPE_DESCRIPTOR:
                if (getIdKind(identifierNode) != NORMAL_ID) {
                    printErrorMsg(identifierNode, NOT_ARRAY);
                }
                // ignore array dims, treat as normal id
                setIdKind(identifierNode, NORMAL_ID);
                identifierNode->dataType = getTypeDescriptor(sym)->properties.dataType;
                break;
            case ARRAY_TYPE_DESCRIPTOR:
                checkArrayDereference(identifierNode, matchDimensions);
                break;
            default:
                ;
        }
    }
}


void processParamListNode(AST_NODE* paramListNode)
{
}


void processVarDeclListNode(AST_NODE* varDeclListNode)
{
    visitChildren(varDeclListNode);
}


void processStmtListNode(AST_NODE* stmtListNode)
{
    visitChildren(stmtListNode);
}


void processNonemptyAssignExprListNode(AST_NODE* node)
{
    visitChildren(node);
}


void processNonemptyRelOpExprListNode(AST_NODE* node)
{
    visitChildren(node);
}


int checkRedeclared(AST_NODE *idNode, int checkLocal)
{
    // return 1 if check passed
    char *id = getIdName(idNode);
    if (checkLocal && !declaredLocally(id)) return 1;
    if (!checkLocal && retrieveSymbol(id) == NULL) return 1;
    printErrorMsg(idNode, SYMBOL_REDECLARE);
    return 0;
}


char *getIdName(AST_NODE *idNode)
{
    return idNode->semantic_value.identifierSemanticValue.identifierName;
}


SymbolTableEntry *getIdSymtabEntry(AST_NODE *idNode)
{
    return idNode->semantic_value.identifierSemanticValue.symbolTableEntry;
}


void setIdSymtabEntry(AST_NODE *idNode, SymbolTableEntry *sym)
{
    idNode->semantic_value.identifierSemanticValue.symbolTableEntry = sym;
}


IDENTIFIER_KIND getIdKind(AST_NODE *idNode)
{
    return idNode->semantic_value.identifierSemanticValue.kind;
}


void setIdKind(AST_NODE *idNode, IDENTIFIER_KIND kind)
{
    idNode->semantic_value.identifierSemanticValue.kind = kind;
}


void checkArrayDereference(AST_NODE *idNode, int matchDimensions)
{
    SymbolTableEntry *sym = getIdSymtabEntry(idNode);
    AST_NODE *dimListNode = idNode->child;
    int dim = 0;
    for (; dimListNode != NULL; dimListNode = dimListNode->rightSibling) {
        visit(dimListNode);
        if (dimListNode->dataType != INT_TYPE) {
            printErrorMsg(dimListNode->parent, ARRAY_SUBSCRIPT_NOT_INT);
        }
        dim++;
    }
    TypeDescriptor *typeDesc = sym->attribute->attr.typeDescriptor;
    if (matchDimensions) {
        if (dim != typeDesc->properties.arrayProperties.dimension) {
            printErrorMsg(idNode, INCOMPATIBLE_ARRAY_DIMENSION);
        }
        setIdKind(idNode, NORMAL_ID);
    } else {
        if (dim == typeDesc->properties.arrayProperties.dimension) {
            setIdKind(idNode, NORMAL_ID);
        } else if (dim < typeDesc->properties.arrayProperties.dimension) {
            setIdKind(idNode, ARRAY_ID);
        } else {
            // XXX
            setIdKind(idNode, NORMAL_ID);
        }
    }
    idNode->dataType = getTypeDescriptor(sym)->properties.arrayProperties.elementType;
}


TypeDescriptor *getTypeDescriptor(SymbolTableEntry *sym)
{
    return sym->attribute->attr.typeDescriptor;
}
