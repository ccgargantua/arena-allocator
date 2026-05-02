# Detect Windows (MinGW/MSYS2/Cygwin)
ifdef ComSpec
    RM = del /q
    EXT = .exe
    NULL = nul
else
    RM = rm -f
    EXT =
    NULL = /dev/null
endif

COMPLIANCE_FLAGS = -std=c89 -Wstrict-prototypes -Wold-style-definition \
                  -Wmissing-prototypes -Wmissing-declarations \
                  -Wdeclaration-after-statement
CFLAGS = -Og -g -Werror -Wall -Wextra -pedantic

tests:
	$(CC) $(CFLAGS) $(COMPLIANCE_FLAGS) -c -o arena.o arena.c
	$(CC) $(CFLAGS) -std=c11 -o test$(EXT) tests.c rktest.c arena.o -lm

%: code_examples/%.c
	$(CC) $(CFLAGS) -o $@$(EXT) $<

test: tests
	$(MAKE) --no-print-directory clean

clean:
	$(RM) test$(EXT) 2>$(NULL) || true
	$(RM) *.o 2>$(NULL) || true
