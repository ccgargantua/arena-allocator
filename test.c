#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int passed_tests = 0;
static int total_tests = 0;

#define TEST(exp, desc) do {                         \
    ++total_tests;                                   \
    if( (exp) ) {                                    \
        ++passed_tests;                              \
    }                                                \
    else                                             \
        fprintf(stderr, "Failed test '%s'\n", desc); \
} while(0)

#define TEST_EQUAL(a, b) do{TEST(a == b, #a " != " #b);}while(0)

#define TEST_FATAL(exp, desc) TEST(exp, "FATAL: " desc); do{if(!(exp))abort();}while(0)

static int temp_passed;
static int temp_total;

#define REPORT(suite, name) do {                                                                             \
    temp_passed = passed_tests;                                                                              \
    temp_total = total_tests;                                                                                \
    suite();                                                                                                 \
    fprintf(stderr, "Passed %d/%d tests in '%s'\n", passed_tests-temp_passed, total_tests-temp_total, name); \
} while(0)

#define ARENA_IMPLEMENTATION
#define ARENA_SUPPRESS_MALLOC_WARN
#include "arena.h"

void test_arena_create(void);
void test_arena_alloc(void);
void test_arena_alloc_aligned(void);
void test_arena_clear(void);

int main()
{
    REPORT(test_arena_create, "Arena creation suite");
    REPORT(test_arena_alloc, "Arena unaligned allocation suite");
    REPORT(test_arena_alloc_aligned, "Arena aligned allocation suite");
    REPORT(test_arena_clear, "Arena clearing suite");

    fprintf(stderr, "\nFinished.Passed %d/%d tests.\n", passed_tests, total_tests);

    return 0;
}


void test_arena_create(void)
{
    Arena *arena = arena_create(32);
    TEST_FATAL(arena != NULL, "Arena was NULL after creation. Fatal.");
    TEST_FATAL(arena->region != NULL, "Arena region was NULL");
    TEST_EQUAL(arena->size, 32);
    TEST_EQUAL(arena->index, 0);
    arena_destroy(arena);
}


void test_arena_alloc(void)
{
    Arena *arena = arena_create(13 + sizeof(long) * 3);

    char *char_array = arena_alloc(arena, 13);
    TEST_FATAL(char_array != NULL, "char array allocated from arena was NULL.");

    memcpy(char_array, "Hello, world!", 13);
    TEST_EQUAL(char_array[0], 'H');
    TEST_EQUAL(char_array[1], 'e');
    TEST_EQUAL(char_array[2], 'l');
    TEST_EQUAL(char_array[4], 'o');
    TEST_EQUAL(char_array[7], 'w');
    TEST_EQUAL(char_array[9], 'r');
    TEST_EQUAL(char_array[10], 'l');
    TEST_EQUAL(char_array[11], 'd');

    TEST_EQUAL(arena->index, 13);

    long *long_array = arena_alloc(arena, sizeof(long) * 3);
    TEST_FATAL(long_array != NULL, "long array allocated from arena was NULL.");

    long expected[3] = {999, 9999, 99999};
    memcpy(long_array, expected, sizeof(long) * 3);
    TEST_EQUAL(long_array[0], 999);
    TEST_EQUAL(long_array[1], 9999);
    TEST_EQUAL(long_array[2], 99999);

    TEST_EQUAL(arena->index, arena->size);

    char *should_not_be_allocated = arena_alloc(arena, 1);
    TEST_EQUAL(should_not_be_allocated, NULL);

    arena_destroy(arena);
}


void test_arena_alloc_aligned(void)
{
    Arena *arena = arena_create(64);

    arena_alloc_aligned(arena, 8, 4);
    TEST_EQUAL(arena->index, 8);

    arena_alloc_aligned(arena, 3, 4);
    TEST_EQUAL(arena->index, 11);
    
    arena_alloc_aligned(arena, 12, 4);
    TEST_EQUAL(arena->index, 24);
    
    arena_alloc_aligned(arena, 3, 4);
    TEST_EQUAL(arena->index, 27);
    
    arena_alloc_aligned(arena, 1, 4);
    TEST_EQUAL(arena->index, 29);

    arena_destroy(arena);
}


void test_arena_clear(void)
{
    Arena *arena = arena_create(10);
    arena->index = 5;
    arena_clear(arena);
    TEST_EQUAL(arena->index, 0);
    arena_destroy(arena);
}
