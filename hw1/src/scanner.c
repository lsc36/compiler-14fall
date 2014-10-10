#include <stdio.h>
#include "type.h"
#include "scanner.h"

/*********************************************
  Scanning
 *********************************************/
// scan與parse一起做引起許多不必要的複雜度，unget回buffer的作法尤其醜陋，且導致last_token的不精確
// 若將scan完的結果先行存下，之後再去擷取，會簡單些
Token getNumericToken( FILE *source, char c )
{
    Token token;
    int i = 0;

    while( isdigit(c) ) {
        token.tok[i++] = c;
        c = (char)fgetc(source);
    }

    if( c != '.' ){
        ungetc(c, source);
        token.tok[i] = '\0';
        token.type = IntValue;
        return token;
    }

    token.tok[i++] = '.';

    c = (char)fgetc(source);
    if( !isdigit(c) ){
        ungetc(c, source);
        printf("Expect a digit : %c\n", c);
        exit(1);
    }

    while( isdigit(c) ){
        token.tok[i++] = c;
        c = (char)fgetc(source);
    }

    ungetc(c, source);
    token.tok[i] = '\0';
    token.type = FloatValue;
    return token;
}

Token getAlphabetToken( FILE *source, char c ) {
    int i = 0;
    Token token = {.type = Alphabet};
    token.tok[i++] = c;
    while (islower(c = (char)fgetc(source))) {
        token.tok[i++] = c;
    }
    ungetc(c, source);
    token.tok[i] = '\0';
    return token;
}

Token scanner( FILE *source )
{
    char c;
    Token token;

    while( !feof(source) ){
        c = (char)fgetc(source);

        while( isspace(c) ) c = (char)fgetc(source);

        if( isdigit(c) ) {
            return getNumericToken(source, c);
        }
        token.tok[0] = c;
        token.tok[1] = '\0';
        if( islower(c) ){
            char nc = (char)fgetc(source);
            if( c == 'f' && isspace(nc)){
                token.type = FloatDeclaration;
            }
            else if( c == 'i' && isspace(nc)){
                token.type = IntegerDeclaration;
            }
            else if( c == 'p' && isspace(nc)){
                strcpy(token.tok, "p ");            // 讓之後unget整個token的時候可以unget整個回來
                token.type = PrintOp;
            }
            else{
                ungetc(nc, source);
                token = getAlphabetToken(source, c);
            }
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
        return token;
    }

    token.tok[0] = '\0';
    token.type = EOFsymbol;
    // 下面這行大概不用了
    return token;
}
