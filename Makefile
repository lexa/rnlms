CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -ggdb
OBJ=rnlms.o tester.o  CircularBuffer.o
HEAD=global.h rnlms.h CircularBuffer.h

all:main
main: $(HEAD) $(OBJ)
		$(CC) $(OBJ) $(CFLAGS) -o main -lm

clean:
		rm -f *.o main
