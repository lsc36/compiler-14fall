CC = gcc
CFLAGS = -Wall
OBJS = scanner.o parser.o builder.o checker.o codegen.o AcDc.o

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o AcDc

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm AcDc *.o
