#!/usr/bin/make
.SUFFIXES:
.PHONY: all run pack clean

SRC = dict.c filesystem.c Vector.c
HED = dict.h filesystem.h Vector.h

TAR = fss

CFLAGS = -std=gnu11 -c -g -Os -Wall -Werror
LFLAGS = -pthread

%.o: $(SRC) $(HED)
	$(CC) $(CFLAGS) $< -o $@

fss: $(SRC)
	$(CC) $(LFLAGS) $^ -o $@

all: $(TAR)

run: all
	./$(TAR)
