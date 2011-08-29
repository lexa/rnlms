CC=gcc
CFLAGS=-Wall -std=gnu89 -O0 -ggdb -Wall -Wextra -Wformat=2 -pedantic -Winit-self -Wstrict-overflow=5 -Wcast-qual -Wcast-align -Wconversion -Waggregate-return 
# CFLAGS=-Wall -std=c99 -O0 -ggdb
OBJS=rnlms.o tester.o  CircularBuffer.o global.o 
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
