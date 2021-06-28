CC:=gcc
CFlags:=-Wall
SRC:= $(wildcard *.c)
OBJ:=$(SRC:.c=.o)
LIB:=${wildcard *.h}
BIN:=calculator
PREFIX ?= /usr

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFlags) -o $@ $^

install:
	install -m 644 $(LIB) $(DESTDIR)$(PREFIX)/include

uninstall:
	$(RM) $(PREFIX)/include/$(LIB)

clean:
	rm -f $(BIN) $(OBJ)
