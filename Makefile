.POSIX:
.PHONY: all

CC = cc
CFLAGS = -O2 -g -Wall -Werror -Wextra -Winit-self -Wuninitialized -pedantic -Wunreachable-code

all: gg

gg: main.c
	$(CC) $(CFLAGS) main.c -o gg 
