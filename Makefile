CFLAGS := -Wall -Wextra -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement -pedantic -std=c89 -g
EXAMPLES_C = $(wildcard code_examples/*.c)
EXAMPLES_OUT = $(patsubst code_examples/%.c,%,$(EXAMPLES_C))

tests:
	@$(CC) $(CFLAGS) -o test test.c

examples: $(EXAMPLES_OUT)

%: code_examples/%.c
	@$(CC) -o $@ $<

test: tests
	valgrind ./test
	@$(MAKE) --no-print-directory clean

clean:
	@rm -f test
	@rm -f ./example*