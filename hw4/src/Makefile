TARGET = parser
OBJECT = parser.tab.c parser.tab.o lex.yy.c alloc.o functions.o semanticAnalysis.o symbolTable.o
OUTPUT = parser.output parser.tab.h testSymbolTable
CC = gcc -g
LEX = flex
YACC = bison -v
YACCFLAG = -d
LIBS = -lfl 

all: parser testSymbolTable

parser: parser.tab.o alloc.o functions.o symbolTable.o semanticAnalysis.o 
	$(CC) -o $(TARGET) parser.tab.o alloc.o functions.o symbolTable.o semanticAnalysis.o $(LIBS)

parser.tab.o: parser.tab.c lex.yy.c alloc.o functions.c symbolTable.o semanticAnalysis.o
	$(CC) -c parser.tab.c
    
semanticAnalysis.o: semanticAnalysis.c symbolTable.o
	$(CC) -c semanticAnalysis.c

test: testSymbolTable
	./testSymbolTable
	
testSymbolTable: testSymbolTable.c symbolTable.o
	$(CC) -std=c11 -o testSymbolTable testSymbolTable.c symbolTable.o

symbolTable.o: symbolTable.c
	$(CC) -std=c11 -c symbolTable.c

lex.yy.c: lexer3.l
	$(LEX) lexer3.l

parser.tab.c: parser.y 
	$(YACC) $(YACCFLAG) parser.y

alloc.o: alloc.c
	$(CC) -c alloc.c
	
functions.o: functions.c
	$(CC) -c functions.c

clean:
	rm -f $(TARGET) $(OBJECT) $(OUTPUT)

graph:
	dot -Tpng AST_Graph.gv > AST_Graph.png
