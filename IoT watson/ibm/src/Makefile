SRC:=$(wildcard *.c)
OBJS:=$(SRC:.c=.o)
LDFLAGS:=-lpaho-mqtt3as -liotp-as-device -lblobmsg_json -lubox -lubus -luci
BIN:=ibm

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(rm) $(BIN)