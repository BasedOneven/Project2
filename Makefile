CC=gcc
CFLAGS=-I.

wzip: main.o
	$(CC) -o wzip main.o