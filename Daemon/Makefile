CC:=gcc
CFLAGS:=-Wall -Werror -pedantic
CURDIR:=$(shell pwd)
EXTRAFLAGS:=-shared -fpic
BIN:=mover
LIB:=libdynamic.so
PREFIX ?= /usr
LDFLAGS:=-L$(CURDIR)

.phony = all clean

all: $(LIB) $(BIN)

$(LIB): ./lib/dynamiclibrary.c
		$(CC) $(CFLAGS) $(EXTRAFLAGS) -o $@ $<

$(BIN): main.c
		$(CC) -g -D_DEFAULT_SOURCE $(CFLAGS) -o $@ $< $(LDFLAGS) -ldynamic  -Wl,-rpath=$(CURDIR)

clean:
		$(RM) $(BIN) $(LIB)
