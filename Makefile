CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -ggdb
OBJ=rnlms.o tester.o 
HEAD=global.h rnlms.h
all: $(HEAD) $(OBJ)
		$(CC) $(OBJ) $(CFLAGS) -o main -lm