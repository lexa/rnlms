CC=gcc
CFLAGS=-Wall -std=gnu89 -O0 -ggdb -Wall -Wextra -Wformat=2 -pedantic -Winit-self -Wstrict-overflow=5 -Wcast-qual -Wcast-align -Wconversion -Waggregate-return 
# CFLAGS=-Wall -std=c99 -O0 -ggdb
OBJS=rnlms.o tester.o  CircularBuffer.o global.o utils.o
HEAD=global.h rnlms.h CircularBuffer.h utils.h tester.h

all:main

# ${OBJS}: *.c *.h Makefile
# 		$(CC) %.c %.h $(CFLAGS)

# %.o: %.c 
# 		$(CC) $%.c $%.h $(CFLAGS)


$(OBJS): %.o: %.c %.h Makefile
		$(CC) -c $(CFLAGS) $< -o $@

main: $(HEAD) $(OBJS)
		$(CC) $(OBJS) $(CFLAGS) -o $@ -lm

# .PHONY: test
# test:
# 		bash -c "cd Debug; ../main | awk 'BEGIN {print \"plot \\\"-\\\" with lines \"} {print \$$1} END{print  \"e\npause mouse keypress \\\"Hit return to continue\\\" \"} ' | gnuplot ; "

.PHONY: test
test:
		bash -c "cd Debug; ../main"
		gnuplot -e "plot 'Debug/error.dat' binary format='%short' using 1 with lines; pause mouse keypress \"Hit return to continue\";"


.PHONY: clean
clean:
		rm -f *.o main
