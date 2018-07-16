#!/usr/bin/make
.SUFFIXES:
.PHONY: all run pack clean

CSRC = $(wildcard src/common/*.c)
FSRC = src/filesystem/DedupLayer.c
CHED = $(wildcard include/common/*.h)
FHED = include/filesystem/DedupLayer.h

TAR = fss

CFLAGS = -std=gnu11 -c -g -Os -Wall -Werror
LFLAGS = -pthread

%.o: $(CSRC) $(FSRC) $(CHED) $(FHED)
	$(CC) $(CFLAGS) $< -o $@

fss: $(CSRC) $(FSRC)
	$(CC) $(LFLAGS) $^ -o $@

all: $(TAR)

run: all
	./$(TAR)
