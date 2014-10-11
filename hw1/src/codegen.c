#include <stdio.h>
#include <string.h>
#include "type.h"
#include "codegen.h"


/***********************************************************************
  Code generation
 ************************************************************************/
void fprint_op( FILE *target, ValueType op )
{
    switch(op){
        case MinusNode:
            fprintf(target,"-\n");
            break;
        case PlusNode:
            fprintf(target,"+\n");
            break;
        case MulNode:
            fprintf(target, "*\n");
            break;
        case DivNode:
            fprintf(target, "/\n");
            break;
        default:
            fprintf(target,"Error in fprintf_op ValueType = %d\n",op);
            break;
    }
}

// 到codegen才來查找所屬register
char find_register(char *s, SymbolTable *table){
    for (int i = 0; i < table->count; i++) {
        if (strcmp(s, table->name[i]) == 0) {
            return (char)(i + 'a');
        }
    }
    printf("Error, can not find variable %s", s);
    exit(1);
}

void fprint_expr(FILE *target, Expression *expr, SymbolTable *table)
{

    if(expr->leftOperand == NULL){
        char reg;
        switch( (expr->v).type ){
            case Identifier:
                reg = find_register((expr->v).val.id, table);
                fprintf(target,"l%c\n", reg);
                break;
            case IntConst:
                fprintf(target,"%d\n",(expr->v).val.ivalue);
                break;
            case FloatConst:
                fprintf(target,"%f\n", (expr->v).val.fvalue);
                break;
            default:
                fprintf(target,"Error In fprint_left_expr. (expr->v).type=%d\n",(expr->v).type);
                break;
        }
    }
    else if(expr->rightOperand == NULL && expr->v.type == NegNode){    // unary minus
        fprintf(target, "0\n");
        fprint_expr(target, expr->leftOperand, table);
        fprintf(target, "-\n");
    }
    else{
        fprint_expr(target, expr->leftOperand, table);
        if(expr->rightOperand == NULL && expr->v.type == IntToFloatConvertNode) {         // type convert
            fprintf(target, "5k\n");
        }
        else{
            //	fprint_right_expr(expr->rightOperand);
            fprint_expr(target, expr->rightOperand, table);
            fprint_op(target, (expr->v).type);
        }
    }
}

void gencode(Program prog, FILE * target, SymbolTable *table)
{
    Statements *stmts = prog.statements;
    Statement stmt;

    while(stmts != NULL){
        stmt = stmts->first;
        char reg;
        switch(stmt.type){
            case Print:
                reg = find_register(stmt.stmt.variable, table);
                fprintf(target,"l%c\n", reg);
                fprintf(target,"p\n");
                break;
            case Assignment:
                reg = find_register(stmt.stmt.assign.id, table);
                fprint_expr(target, stmt.stmt.assign.expr, table);
                /*
                   if(stmt.stmt.assign.type == Int){
                   fprintf(target,"0 k\n");
                   }
                   else if(stmt.stmt.assign.type == Float){
                   fprintf(target,"5 k\n");
                   }*/
                fprintf(target,"s%c\n", reg);
                fprintf(target,"0 k\n");
                break;
        }
        stmts=stmts->rest;
    }

}
