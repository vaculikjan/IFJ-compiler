CFLAGS=-std=c99 -Wall -pedantic -g
BIN=project
CC=gcc
RM=rm -f

all: stack.o str.o symtable.o interpret.o ilist.o scanner.o parser.o main.o
	$(CC) $(CFLAGS) -o $(BIN) stack.o str.o symtable.o interpret.o ilist.o scanner.o parser.o main.o

clean:
	$(RM) *.o $(BIN)
