#include <stdio.h>
#include "type.h"
#include "checker.h"


/********************************************************************
  Type checking
 *********************************************************************/

void convertType( Expression * old, DataType type )
{
    if(old->type == Float && type == Int){
        printf("error : can't convert float to integer\n");
        return;
    }
    if(old->type == Int && type == Float){
        Expression *tmp = (Expression *)malloc( sizeof(Expression) );
        if(old->v.type == Identifier)
            printf("convert to float %s \n",old->v.val.id);
        else
            printf("convert to float %d \n", old->v.val.ivalue);
        tmp->v = old->v;
        tmp->leftOperand = old->leftOperand;
        tmp->rightOperand = old->rightOperand;
        tmp->type = old->type;

        Value v;
        v.type = IntToFloatConvertNode;
        v.val.op = IntToFloatConvert;
        old->v = v;
        old->type = Int;
        old->leftOperand = tmp;
        old->rightOperand = NULL;
    }
}

DataType generalize( Expression *left, Expression *right )
{
    if(left->type == Float || right->type == Float){
        printf("generalize : float\n");
        return Float;
    }
    printf("generalize : int\n");
    return Int;
}

DataType lookup_table( SymbolTable *table, char *s )
{
    int i;
    for (i = 0; i < table->count; i++) {
        if(strcmp(s, table->name[i]) == 0) {
            return table->table[i];
        }
    }
    printf("Error : identifier %s is not declared\n", s);//error
    return Notype;
    //exit(1);
}

void checkexpression( Expression * expr, SymbolTable * table )
{
    char *s;
    if(expr->leftOperand == NULL && expr->rightOperand == NULL){
        switch(expr->v.type){
            case Identifier:
                s = expr->v.val.id;
                printf("identifier : %s\n",s);
                expr->type = lookup_table(table, s);
                break;
            case IntConst:
                printf("constant : int\n");
                expr->type = Int;
                break;
            case FloatConst:
                printf("constant : float\n");
                expr->type = Float;
                break;
                //case PlusNode: case MinusNode: case MulNode: case DivNode:
            default:
                break;
        }
    }
    else if (expr->rightOperand == NULL){
        checkexpression(expr->leftOperand, table);
        expr->type = expr->leftOperand->type;
    }
    else{
        Expression *left = expr->leftOperand;
        Expression *right = expr->rightOperand;

        checkexpression(left, table);
        checkexpression(right, table);

        DataType type = generalize(left, right);
        convertType(left, type);//left->type = type;//converto
        convertType(right, type);//right->type = type;//converto
        expr->type = type;

        // optimization: constant folding
        switch (expr->v.type) {
            case PlusNode:
            case MinusNode:
            case MulNode:
            case DivNode:
                if (type == Int && left->v.type == IntConst && right->v.type == IntConst) {
                    int *c = &expr->v.val.ivalue, *a = &left->v.val.ivalue,
                        *b = &right->v.val.ivalue; // *c = *a <op> *b
                    char op;
                    switch (expr->v.type) {
                        case PlusNode:  *c = *a + *b; op = '+'; break;
                        case MinusNode: *c = *a - *b; op = '-'; break;
                        case MulNode:   *c = *a * *b; op = '*'; break;
                        case DivNode:   *c = *a / *b; op = '/'; break;
                        default: ;
                    }
                    expr->v.type = IntConst;
                    expr->leftOperand = expr->rightOperand = NULL;
                    printf("optimize: %d %c %d = %d\n", *a, op, *b, *c);
                }
                else if (type == Float && left->v.type == FloatConst && right->v.type == FloatConst) {
                    float *c = &expr->v.val.fvalue, *a = &left->v.val.fvalue,
                        *b = &right->v.val.fvalue; // *c = *a <op> *b
                    char op;
                    switch (expr->v.type) {
                        case PlusNode:  *c = *a + *b; op = '+'; break;
                        case MinusNode: *c = *a - *b; op = '-'; break;
                        case MulNode:   *c = *a * *b; op = '*'; break;
                        case DivNode:   *c = *a / *b; op = '/'; break;
                        default: ;
                    }
                    expr->v.type = FloatConst;
                    expr->leftOperand = expr->rightOperand = NULL;
                    printf("optimize: %.5f %c %.5f = %.5f\n", *a, op, *b, *c);
                }
                break;
            default:
                break;
        }
    }
}

void checkstmt( Statement *stmt, SymbolTable * table )
{
    if(stmt->type == Assignment){
        AssignmentStatement assign = stmt->stmt.assign;
        printf("assignment : %s \n",assign.id);
        checkexpression(assign.expr, table);
        stmt->stmt.assign.type = lookup_table(table, assign.id);
        if (assign.expr->type == Float && stmt->stmt.assign.type == Int) {
            printf("error : can't convert float to integer\n");
        } else {
            convertType(assign.expr, stmt->stmt.assign.type);
        }
    }
    else if (stmt->type == Print){
        printf("print : %s \n",stmt->stmt.variable);
        lookup_table(table, stmt->stmt.variable);
    }
    else printf("error : statement error\n");//error
}

void check( Program *program, SymbolTable * table )
{
    Statements *stmts = program->statements;
    while(stmts != NULL){
        checkstmt(&stmts->first,table);
        stmts = stmts->rest;
    }
}
