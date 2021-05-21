CC:=gcc
CFlags:=-Wall
SRC:= $(wildcard *.c)
OBJ:=$(SRC:.c=.o)
BIN:=main
PREFIX ?= /usr

all: LIB MAIN $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFlags) -o $@ $^

LIB: file_reader.c
	$(CC) $(CFlags) -c -o file_reader.o $<

MAIN: main.c
	$(CC) $(CFlags) -c -o main.o $<
	
clean:
	rm -f $(BIN) $(OBJ)
