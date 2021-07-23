SRC:=$(wildcard *.c)
OBJS:=$(SRC:.c=.o)
LDFLAGS:=-lmosquitto -luci -lsqlite3 -ljson-c -lcurl
BIN:=mqtt

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(rm) $(BIN)