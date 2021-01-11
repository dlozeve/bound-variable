CC = gcc
CFLAGS = -Wall -Werror -pedantic -g

.PHONY: all
all: um

um: um.c
	$(CC) $< -o $@ $(CFLAGS)
