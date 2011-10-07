CC=gcc
CFLAGS=-Wall -std=gnu89 -O0 -ggdb -Wall -Wextra -Wformat=2 -pedantic -Winit-self -Wstrict-overflow=5 -Wcast-qual -Wcast-align -Wconversion -Waggregate-return  -fPIC
# CFLAGS=-Wall -std=c99 -O0 -ggdb
OBJS=rnlms.o  CircularBuffer.o global.o
HEAD=global.h rnlms.h CircularBuffer.h 
# BIN_FILES=$(wildcard g165/*.dat)
# DAT_FILES=$(BIN_FILES=$:.dat=_ccs.dat)
TESTER_OBJS=tester.c utils.c
TESTER_HEAD=tester.h utils.h

all:main 

# ${OBJS}: *.c *.h Makefile
# 		$(CC) %.c %.h $(CFLAGS)

# %.o: %.c 
# 		$(CC) $%.c $%.h $(CFLAGS)


$(OBJS): %.o: %.c %.h Makefile
		$(CC) -c $(CFLAGS) $< -o $@

librnlms.so: $(HEAD) $(OBJS) 
		$(CC) -shared $(OBJS) $(CFLAGS) -o $@ -lm

main: librnlms.so $(TESTER_OBJS)  $(TESTER_HEAD)
		$(CC) $? -o $@ -I./ -lrnlms -L ./


.PHONY: test
test:

		bash -c "export LD_LIBRARY_PATH=`pwd`; cd Debug; ../main"
		gnuplot -e "set terminal x11; plot 'Debug/error.dat' binary format='%short' using 1 with lines; pause mouse keypress \"Hit return to continue\";"


.PHONY: clean
clean:
		rm -f *.o main *.so

