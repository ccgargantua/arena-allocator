/*
Tests are contained in testing suite functions, the forware declarations
of which are bundled together. A testing suite function should exist for
each function within `arena.h`, and should be placed relative to where
the function they are testing is placed in regard to the functions around
it for both forward declarations, implementations, and `SUITE`s.

After implementing a testing suite, you should use the `SUITE` in
`main()`.

Within the testing suite, the following macros should be used:

TEST_FATAL(exp, desc)     | TEST_FATAL should be used whenever a failure
                            could cause future tests to crash the process.
                            If exp evaluates to false (0), the test executable
                            will abort completely and the desc string will be
                            printed.

                            EX/ An `arena_create` failing and returning NULL
                                could cause an access within the test to
                                segfault.

TEST_EQUAL(a, b)          | TEST_EQUAL will fail when a != b

TEST_NULL(a)              | TEST_NULL will fail when a != NULL

TEST_NOT_NULL(a)          | TEST_NOT_NULL will fail when a == NULL

TEST_ARRAY_EQUAL(a, b, s) | TEST_ARRAY_EQUAL will fail if any elements differ
                            in two arrays a, b that are both of size s
*/

#include "test.h"


void test_arena_create(void);
void test_arena_alloc(void);
void test_arena_alloc_aligned(void);
void test_arena_copy(void);
void test_arena_clear(void);
void test_arena_get_allocation_struct(void);
void test_arena_add_allocation(void);
void test_arena_delete_allocation_list(void);


int main(void)
{
    SUITE(test_arena_create, "Arena creation suite");
    SUITE(test_arena_alloc, "Arena unaligned allocation suite");
    SUITE(test_arena_alloc_aligned, "Arena aligned allocation suite");
    SUITE(test_arena_copy, "Arena copy suite");
    SUITE(test_arena_clear, "Arena clearing suite");
    SUITE(test_arena_get_allocation_struct, "Arena debug method 'arena_get_allocation_struct' suite");
    SUITE(test_arena_add_allocation, "Arena debug method 'arena_add_allocation' suite");
    SUITE(test_arena_delete_allocation_list, "Arena debug method 'arena_delete_allocation_list' suite");

    fprintf(stderr, "\nFinished. Passed %d/%d tests.\n", passed_tests, total_tests);

    return 0;
}


void test_arena_create(void)
{
    Arena *arena = arena_create(0);
    TEST_NULL(arena);
    arena = arena_create(32);
    TEST_FATAL(arena != NULL, "Arena was NULL after creation. Fatal.");
    TEST_FATAL(arena->region != NULL, "Arena region was NULL");
    TEST_EQUAL(arena->allocations, 0);
    TEST_NULL(arena->head_allocation);
    TEST_EQUAL(arena->size, 32);
    TEST_EQUAL(arena->index, 0);
    arena_destroy(arena);
}


void test_arena_alloc(void)
{
    Arena *arena = arena_create(13 + sizeof(long) * 3);
    char *char_array = arena_alloc(arena, 13);
    long *long_array;
    long expected_long_array[3] = {999, 9999, 99999};
    char *should_not_be_allocated;

    TEST_NULL(arena_alloc(arena, 0));

    TEST_FATAL(char_array != NULL, "char array allocated from arena was NULL.");

    memcpy(char_array, "Hello, world!", 13);
    TEST_ARRAY_EQUAL(char_array, "Hello, world!", 13);
    TEST_EQUAL(arena->index, 13);

    long_array = arena_alloc(arena, sizeof(long) * 3);
    TEST_FATAL(long_array != NULL, "long array allocated from arena was NULL.");

    memcpy(long_array, expected_long_array, sizeof(long) * 3);
    TEST_ARRAY_EQUAL(long_array, expected_long_array, 3);
    TEST_EQUAL(arena->index, arena->size);

    /* Failures */
    
    TEST_NULL(arena_alloc(NULL, 0));

    should_not_be_allocated = arena_alloc(arena, 1);
    TEST_NULL(should_not_be_allocated);

    arena_destroy(arena);
}


void test_arena_alloc_aligned(void)
{
    Arena *arena = arena_create(64);

    TEST_NULL(arena_alloc_aligned(arena, 0, 0));

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

    TEST_NULL(arena_alloc_aligned(arena, 100, 0));

    arena_destroy(arena);
}


void test_arena_copy(void)
{
    Arena *arena_src = arena_create(1024);
    Arena *arena_dest = arena_create(500);
    char *src_array;

    TEST_FATAL(arena_src != NULL, "Source arena creation failed!");
    TEST_FATAL(arena_dest != NULL, "Destination arena creation failed!");

    TEST_EQUAL(arena_copy(NULL, arena_src), 0);
    TEST_EQUAL(arena_copy(arena_dest, NULL), 0);
    TEST_EQUAL(arena_copy(arena_dest, arena_src), 0);

    src_array = arena_alloc(arena_src, 3);
    TEST_FATAL(src_array != NULL, "Source arena allocation failed!");
    src_array[0] = 'a';
    src_array[1] = 'b';
    src_array[2] = 'c';
    TEST_EQUAL(arena_copy(arena_dest, arena_src), 3);
    TEST_ARRAY_EQUAL(arena_dest->region, arena_src->region, 3);
    TEST_EQUAL(arena_dest->index, 3);

    arena_src->index = arena_src->size;
    TEST_EQUAL(arena_copy(arena_dest, arena_src), arena_dest->size);
    TEST_EQUAL(arena_dest->size, arena_dest->index);

    arena_destroy(arena_src);
    arena_destroy(arena_dest);
}


void test_arena_clear(void)
{
    Arena *arena = arena_create(10);
    arena->index = 5;
    arena_clear(arena);
    TEST_EQUAL(arena->index, 0);
    arena_destroy(arena);
}


void test_arena_get_allocation_struct(void)
{
    Arena *arena = arena_create(64);
    char *ptr = arena_alloc(arena, 8);
    char fake = 'a';
    Arena_Allocation *allocation_struct = arena_get_allocation_struct(arena, ptr);

    TEST_NULL(arena_get_allocation_struct(NULL, ptr));
    TEST_NULL(arena_get_allocation_struct(arena, NULL));

    TEST_FATAL(arena != NULL, "Arena was NULL after creation.");

    TEST_FATAL(ptr != NULL, "Pointer was NULL after creation.");

    TEST_FATAL(allocation_struct != NULL, "Allocation struct could not be found through pointer comparison.");
    TEST_EQUAL(allocation_struct->index, 0);
    TEST_EQUAL(allocation_struct->size, 8);
    TEST_NULL(arena_get_allocation_struct(arena, &fake));

    arena_destroy(arena);
}


void test_arena_add_allocation(void)
{
    Arena *arena = arena_create(1024);

    arena_alloc(arena, 10);
    TEST_FATAL(arena->head_allocation != NULL, "Arena head allocation was NULL after first allocation.");
    TEST_EQUAL(arena->head_allocation->size, 10);
    TEST_EQUAL(arena->head_allocation->next, NULL);
    TEST_EQUAL(arena->allocations, 1);

    arena_alloc(arena, 15);
    TEST_FATAL(arena->head_allocation->next != NULL, "Arena end allocation was NULL after first allocation.");
    TEST_EQUAL(arena->head_allocation->next->size, 15);
    TEST_EQUAL(arena->head_allocation->next->next, NULL);
    TEST_EQUAL(arena->allocations, 2);

    arena_alloc(arena, 1);
    TEST_FATAL(arena->head_allocation->next->next != NULL, "Arena end allocation was NULL after first allocation.");
    TEST_EQUAL(arena->head_allocation->next->next->size, 1);
    TEST_EQUAL(arena->head_allocation->next->next->next, NULL);
    TEST_EQUAL(arena->allocations, 3);

    arena_destroy(arena);
}


void test_arena_delete_allocation_list(void)
{
    Arena *arena = arena_create(1024);
    arena_alloc(arena, 10);
    arena_alloc(arena, 15);
    arena_alloc(arena, 1);
    arena_delete_allocation_list(arena);
    TEST_NULL(arena->head_allocation);
    TEST_EQUAL(arena->allocations, 0);

    arena_destroy(arena);
}