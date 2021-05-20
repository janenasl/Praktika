CC:=gcc
CFlags:=-Wall
OBJS:=main.o math_functions.o
LIB:=math_functions.h
BIN:=calculator
PREFIX ?= /usr

all: $(BIN)

$(BIN): main.o math_functions.o
	$(CC) $(CFlags) -o $@ $^

install:a
	install -m 644 $(LIB) $(DESTDIR)$(PREFIX)/include

uninstall:
	$(RM) $(PREFIX)/include/$(LIB)

clean:
	rm -f $(BIN) $(OBJS)
