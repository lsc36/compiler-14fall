TARGET = parser
OBJECT = parser.tab.o alloc.o functions.o semanticAnalysis.o symbolTable.o offset.o
OUTPUT = parser.output parser.tab.h
CC = gcc -g
LEX = flex
YACC = bison -v
YACCFLAG = -d
LIBS = -lfl

parser: $(OBJECT)
	$(CC) -o $(TARGET) $(OBJECT) $(LIBS)

%.o: %.c
	$(CC) -c $<

lex.yy.c: lexer3.l
	$(LEX) lexer3.l

parser.tab.c: parser.y
	$(YACC) $(YACCFLAG) parser.y

parser.tab.o: lex.yy.c

clean:
	rm -f $(TARGET) $(OBJECT) $(OUTPUT) lex.yy.c parser.tab.c
