CC = gcc
CFLAGS = -Wall -Werror -pedantic -O3

.PHONY: all
all: um

um: src/um.c
	$(CC) $^ -o $@ $(CFLAGS)
