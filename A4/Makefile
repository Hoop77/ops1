#!/usr/bin/make
.SUFFIXES:
.PHONY: all run pack clean

TAR = ult

COMMON_SRC = $(wildcard src/common/*.c)
ULT_SRC = $(wildcard src/ult/*.c)
# This is our test source folder.
TEST_SRC = $(wildcard test/*.c)

COMMON_OBJ = $(COMMON_SRC:%.c=%.o)
ULT_OBJ = $(ULT_SRC:%.c=%.o)
# Server for UltTest.c
TEST_SERVER_OBJ = test/TestServer.o
# This is the object to test / where main() is.
TEST_OBJ = test/UltTest.o

PCK = lab-4.zip

CFLAGS = -std=gnu11 -c -g -Os -Wall -Werror -Wno-unused-value -Wno-deprecated

%.o: %.c %.h
	$(CC) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

ult: $(COMMON_OBJ) $(ULT_OBJ) $(TEST_OBJ)
	$(CC) $^ -o $@

test_server: $(COMMON_OBJ) $(ULT_OBJ) $(TEST_SERVER_OBJ)
	$(CC) $^ -o $@

all: $(TAR)

run: all
	./$(TAR)

pack: clean
	zip -r $(PCK) src include test Makefile -x "./.*"

clean:
	$(RM) $(RMFILES) $(COMMON_OBJ) $(ULT_OBJ) $(TEST_OBJ) $(TEST_SERVER_OBJ) $(TAR) test_server
