CC:=gcc
CFLAGS:=-Wall
LIB:=calculator.so

all: $(LIB)

$(LIB): math_functions.h
	$(CC) $(CFLAGS) -o $@ $^

install: $(LIB)
	install -m 644 math_functions.h $(DESTDIR)/$(PREFIX)/include

uninstall:

clean:
	rm -f $(LIB)

# https://iq.opengenus.org/create-shared-library-in-cpp/