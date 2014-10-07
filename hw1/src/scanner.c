#include <stdio.h>
#include "type.h"
#include "scanner.h"

/*********************************************
  Scanning
 *********************************************/
Token getNumericToken( FILE *source, char c )
{
    Token token;
    int i = 0;
    if (c == '-') {
        token.tok[i++] = c;
        c = fgetc(source);
    }

    while( isdigit(c) ) {
        token.tok[i++] = c;
        c = fgetc(source);
    }

    if( c != '.' ){
        ungetc(c, source);
        token.tok[i] = '\0';
        token.type = IntValue;
        return token;
    }

    token.tok[i++] = '.';

    c = fgetc(source);
    if( !isdigit(c) ){
        ungetc(c, source);
        printf("Expect a digit : %c\n", c);
        exit(1);
    }

    while( isdigit(c) ){
        token.tok[i++] = c;
        c = fgetc(source);
    }

    ungetc(c, source);
    token.tok[i] = '\0';
    token.type = FloatValue;
    return token;
}

// return true if its TokenType is Alphabet, or IntValue, or FloatValue
int is_value(TokenType t){
    return t == Alphabet || t == IntValue || t == FloatValue;
}

int is_var(char c) {
    return c != 'f' && c != 'i' && c != 'p';
}

Token scanner( FILE *source )
{
    static Token last_token = {.type = Nothing};
    char c;
    Token token;

    while( !feof(source) ){
        c = fgetc(source);

        while( isspace(c) ) c = fgetc(source);

        if( isdigit(c) ) {
            last_token = getNumericToken(source, c);
            return last_token;
        }
        token.tok[0] = c;
        token.tok[1] = '\0';
        if( islower(c) ){
            if (is_var(c))
                token.type = Alphabet;
            else if( c == 'f' )
                token.type = FloatDeclaration;
            else if( c == 'i' )
                token.type = IntegerDeclaration;
            else if( c == 'p' )
                token.type = PrintOp;
            last_token = token;
            return token;
        }

        switch(c){
            case '=':
                token.type = AssignmentOp;
                break;
            case '+':
                token.type = PlusOp;
                break;
            case '-':
                // 當上一個token為value時，將 - 視為二元運算子
                // 否則視為 unary operater
                if (is_value(last_token.type)) {
                    token.type = MinusOp;
                }
                else {
                    last_token = getNumericToken(source, c);
                    return last_token;
                }
                break;
            case '*':
                token.type = MulOp;
                break;
            case '/':
                token.type = DivOp;
                break;
            case EOF:
                token.type = EOFsymbol;
                token.tok[0] = '\0';
                break;
            default:
                printf("Invalid character : %c\n", c);
                exit(1);
        }
        last_token = token;
        return token;
    }

    token.tok[0] = '\0';
    token.type = EOFsymbol;
    // 下面這行大概不用了
    last_token = token;
    return token;
}
