CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -ggdb
OBJS=rnlms.o tester.o  CircularBuffer.o
HEAD=global.h rnlms.h CircularBuffer.h

all:main

# ${OBJS}: *.c *.h Makefile
# 		$(CC) %.c %.h $(CFLAGS)

# %.o: %.c 
# 		$(CC) $%.c $%.h $(CFLAGS)


$(OBJS): %.o: %.c %.h Makefile
		$(CC) -c $(CFLAGS) $< -o $@

main: $(HEAD) $(OBJS)
		$(CC) $(OBJS) $(CFLAGS) -o main -lm

clean:
		rm -f *.o main
