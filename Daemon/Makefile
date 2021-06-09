CC:=gcc
BIN:=main
CFLAGS:=
CURDIR:=$(shell pwd)
SRC:=$(wildcard *.c)
OBJ:=$(SRC:.c=.o)
LDFLAGS:=-L$(CURDIR)

.phony = all clean

all: $(BIN)

$(BIN): $(OBJ)
		$(CC) $(CFLAGS) -o $@ $^ -I$(CURDIR)

clean:
		$(RM) $(BIN) $(OBJ)
