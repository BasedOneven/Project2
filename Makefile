CC=gcc
CFLAGS=-I.

wzip: wzip.o
	$(CC) -o wzip wzip.o