
CC := clang
CFLAGS := -ansi -g

test:
	$(CC) $(CFLAGS) -o test test.c

clean:
	rm -f test