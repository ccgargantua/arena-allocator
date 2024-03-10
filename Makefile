CFLAGS := -Werror -Wall -Wextra -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement -g
EXAMPLES_C = $(wildcard code_examples/*.c)
EXAMPLES_OUT = $(patsubst code_examples/%.c,%,$(EXAMPLES_C))

tests:
	@$(CC) $(CFLAGS) -std=c11 -o test test.c

compliance:
	@$(CC) $(CFLAGS) -pedantic -std=c89 -o test_compliance test_compliance.c
	@rm -f test_compliance

examples: $(EXAMPLES_OUT)

%: code_examples/%.c
	@$(CC) -o $@ $<

test: tests compliance
	valgrind ./test
	@$(MAKE) --no-print-directory clean

clean:
	@rm -f test
	@rm -f ./example*