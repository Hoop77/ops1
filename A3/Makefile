#!/usr/bin/make
.SUFFIXES:
.PHONY: all run pack clean
.SILENT: run

TAR = client server

SERVER_SRC = $(wildcard src/srv/*.c)
CLIENT_SRC = $(wildcard src/cli/*.c)
COMMON_SRC = $(wildcard src/common/*.c)

CLIENT_OBJ = $(CLIENT_SRC:%.c=%.o)
SERVER_OBJ = $(SERVER_SRC:%.c=%.o)
COMMON_OBJ = $(COMMON_SRC:%.c=%.o)

PCK = lab-3.zip

export CFLAGS = -std=gnu11 -c -g -Os -Wall -Werror

%.o: %.c %.h
	$(CC) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

all: $(TAR)

run: all
	server& echo $$! > .srv_pid
	echo "Client Ready:"
	client
	kill `cat .srv_pid`
	$(RM) $(RMFILES) .srv_pid

pack: clean
	zip -r $(PCK) src include Makefile -x "*/.*"

clean:
	$(RM) $(RMFILES) $(CLIENT_OBJ) $(SERVER_OBJ) $(COMMON_OBJ) $(TAR) $(PCK)

client: $(CLIENT_OBJ) $(COMMON_OBJ)
	$(CC) $^ -o $@

server: $(SERVER_OBJ) $(COMMON_OBJ)
	$(CC) $^ -o $@