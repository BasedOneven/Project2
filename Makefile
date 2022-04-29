CC = gcc
CFLAGS = -Wall -I. -O -Werror
SFLAGS = -pthread
OBJS = wzip.o queue.o

.SUFFIXES: .c .o 

all: wzip

wzip: wzip.o queue.o
	$(CC) $(CFLAGS) $(SFLAGS) -o wzip wzip.o queue.o

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f $(OBJS) wzip