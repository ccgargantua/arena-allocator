CFLAGS := -ansi -g
EXAMPLES_C = $(wildcard code_examples/*.c)
EXAMPLES_OUT = $(patsubst code_examples/%.c,%,$(EXAMPLES_C))

tests:
	$(CC) $(CFLAGS) -o test test.c

examples: $(EXAMPLES_OUT)

%: code_examples/%.c
	$(CC) -o $@ $<

clean:
	rm -f test
	rm -f ./example*