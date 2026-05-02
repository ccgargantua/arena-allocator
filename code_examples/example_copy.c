/*
 * Example: Using arena_copy()
 *
 * Demonstrates how to use arena_copy() to allocate and copy
 * a block of memory into the arena.
 *
 * Compile: gcc -o example_copy example_copy.c ../arena.c
 * Run: ./example_copy
 */

#define ARENA_DEBUG
#include "../arena.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    Arena arena;
    char buf[1024];
    arena_init(&arena, buf, sizeof(buf));

    const char *src = "Hello, arena_copy!";
    size_t len = strlen(src) + 1;

    char *dst = arena_copy(&arena, src, len);
    if (!dst) {
        fprintf(stderr, "arena_copy failed\n");
        return 1;
    }

    printf("Copied string: %s\n", dst);
    printf("Length: %zu bytes\n", len);
    printf("Arena used: %zu / %zu bytes\n", arena.index, arena.size);

    char *dst2 = arena_copy(&arena, ", arena_copy works!", 20);
    if (dst2) {
        printf("Second copy: %s\n", dst2);
    }

    arena_destroy(&arena);
    return 0;
}