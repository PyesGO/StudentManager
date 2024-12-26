CC = gcc

CFLAGS += -Wall
MAKEFLAGS += -rR

PHONY += __all
__all: a.out

a.out: *.c
	$(CC) $(CFLAGS) -o $@ $^

PHONY += clean
clean:
	rm -f *.out

.PHONY = PHONY
