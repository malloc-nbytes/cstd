SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
CFLAGS := -Wall -Wextra -std=c99 -g
DEPS := ../cstd.h

.PHONY: all clean run

# Add new bin names.
all: vec str

# Add new object.
vec: vec.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

str: str.o $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Add new run cmds.
run: all
	./vec
	./str

vrun: all
	valgrind ./vec
	valgrind ./str

# Add new remove bins.
clean:
	rm -f *.o vec
