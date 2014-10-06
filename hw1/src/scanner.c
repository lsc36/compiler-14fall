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
            if( c == 'f' )
                token.type = FloatDeclaration;
            else if( c == 'i' )
                token.type = IntegerDeclaration;
            else if( c == 'p' )
                token.type = PrintOp;
            else
                token.type = Alphabet;
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
                //問題：當前面有op時，即為
                //採用預讀，存於靜態函數變數last_token
                token.type = MinusOp;
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
