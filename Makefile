CC = gcc
CFLAGS = -Wall -Werror -pedantic -O3

.PHONY: all
all: um

um: um.c
	$(CC) $< -o $@ $(CFLAGS)
