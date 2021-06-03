CC=gcc
LD_LIBRARY=$(shell pwd)
FLAGS=-g -D_DEFAULT_SOURCE -Wall -Werror -pedantic
OBJ=normal.c

Programs: libdynamic.so telt

telt: $(OBJ)
	$(CC) $(FLAGS) -o main normal.c -L$(LD_LIBRARY) -ldynamic -Wl,-rpath=$(LD_LIBRARY)

libdynamic.so: ./dynamiclibrary.c ./dynamiclibrary.h
	$(CC) -Wall -shared -fPIC -o libdynamic.so ./dynamiclibrary.c
