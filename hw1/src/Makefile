CC = gcc
CFLAGS = -Wall -std=c99
OBJS = AcDc.o builder.o checker.o codegen.o parser.o scanner.o

all: AcDc

AcDc: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o AcDc

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) AcDc
