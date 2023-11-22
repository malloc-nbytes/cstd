SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
CFLAGS := -Wall -Wextra -std=c99 -g
DEPS := ../cstd.h

.PHONY: all clean run

# Add new bin names.
all: vec funcs str stack

# Add new object.
vec: vec.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

funcs: funcs.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

stack: stack.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

str: str.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Add new run cmds.
run: all
	./vec
	./str
	./funcs
	./stack

vrun: all
	valgrind ./vec
	valgrind ./funcs
	valgrind ./stack
	valgrind ./str

# Add new remove bins.
clean:
	rm -f *.o vec funcs stack str
