SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
CFLAGS := -Wall -Wextra -std=c99 -g

.PHONY: all clean run

# Add new bin names.
all: vec

# Add new object.
vec: vec.o
	$(CC) $(CFLAGS) -o $@ $^

umap: umap.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Add new run cmds.
run: all
	./vec

# Add new remove bins.
clean:
	rm -f *.o vec
