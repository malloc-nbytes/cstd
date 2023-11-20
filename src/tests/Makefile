SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
CFLAGS := -Wall -Wextra -std=c99 -g
DEPS := ../cstd.h

.PHONY: all clean run

# Add new bin names.
all: vec iota

# Add new object.
vec: vec.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

iota: iota.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Add new run cmds.
run: all
	./vec
	./iota

vrun: all
	valgrind ./vec
	valgrind ./iota

# Add new remove bins.
clean:
	rm -f *.o vec
