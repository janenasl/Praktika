CC:=gcc
BIN:=main
CFLAGS:=-Wall -Werror -pedantic
CURDIR:=$(shell pwd)
SRC:=$(wildcard *.c)
OBJ:=$(SRC:.c=.o)
LDFLAGS:=-L$(CURDIR)

.phony = all clean

all: $(BIN)

$(BIN): $(OBJ)
		$(CC) $(CFLAGS) -o $@ $^

clean:
		$(RM) $(BIN) $(OBJ)
