CFLAGS = -Werror -Wall -Wextra
COMPLIANCE_FLAGS = -pedantic -std=c89 -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement -g
EXAMPLES_C = $(wildcard code_examples/*.c)
EXAMPLES_OUT = $(patsubst code_examples/%.c,%,$(EXAMPLES_C))

tests:
	@$(CC) $(CFLAGS) -std=c11 -o test test.c

compliance:
	@echo "C89 compliance check..."
	@$(CC) $(CFLAGS) $(COMPLIANCE_FLAGS) -o test_compliance test_compliance.c
	@rm -f test_compliance
	@echo "C89 compliant."

examples: $(EXAMPLES_OUT)

%: code_examples/%.c
	@$(CC) -o $@ $<

test: tests compliance
	@echo "Running tests under valgrind..."
	valgrind ./test
	@$(MAKE) --no-print-directory clean
	@echo "Testing complete."

clean:
	@echo "Removing executables..."
	@rm -f test
	@rm -f ./example*
	@echo "Executables removed."