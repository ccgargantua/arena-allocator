COMPLIANCE_FLAGS = -std=c89  -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement
CFLAGS = -Og -g -Werror -Wall -Wextra -pedantic

tests:
	@$(CC) $(CFLAGS) $(COMPLIANCE_FLAGS) -c -o arena.o arena.c
	@$(CC) $(CFLAGS) -std=c11 -o test tests.c rktest.c arena.o -lm

examples: $(EXAMPLES_OUT)

%: code_examples/%.c
	@$(CC) -o $@ $<

test: tests
	@echo "Running tests under valgrind..."
	valgrind ./test
	@$(MAKE) --no-print-directory clean
	@echo "Testing complete."

clean:
	@echo "Removing executables..."
	@rm -f test
	@rm -f *.o
	@echo "Executables removed."