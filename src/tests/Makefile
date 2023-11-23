SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
CFLAGS := -Wall -Wextra -std=c99 -g
DEPS := ../cstd.h

.PHONY: all clean run

# Add new bin names.
all: vec funcs str stack pair queue

# Add new object.
vec: vec.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

funcs: funcs.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

stack: stack.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

str: str.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

pair: pair.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

queue: queue.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Add new run cmds.
run: all
	./vec
	./str
	./funcs
	./stack
	./pair
	./queue

vrun: all
	valgrind ./vec
	valgrind ./funcs
	valgrind ./stack
	valgrind ./str
	valgrind ./pair
	valgrind ./queue

# Add new remove bins.
clean:
	rm -f *.o vec funcs stack str pair queue
