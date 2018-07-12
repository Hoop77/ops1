#!/usr/bin/make
.SUFFIXES:
.PHONY: all run pack clean

COMMON_SRC = $(wildcard src/common/*.c)
TCD_SRC = $(wildcard src/tcd/*.c)

COMMON_OBJ = $(COMMON_SRC:%.c=%.o)
TCD_OBJ = $(TCD_SRC:%.c=%.o)

TAR = tcd
PCK = lab-5.zip

CFLAGS = -std=gnu11 -c -g -Os -Wall -Werror
LFLAGS = -pthread

%.o: %.c %.h
	$(CC) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

tcd: $(COMMON_SRC) $(TCD_SRC)
	$(CC) $(LFLAGS) $^ -o $@

all: $(TAR)

run: all
	./$(TAR)

pack: clean
	zip -r $(PCK) src include Makefile -x "./.*"

clean:
	$(RM) $(RMFILES) $(COMMON_OBJ) $(TCD_OBJ) $(TAR)
