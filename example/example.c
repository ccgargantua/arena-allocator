#include <stdio.h>  // printf
#include <string.h> // memcpy

#define ARENA_IMPLEMENTATION
#define ARENA_SUPPRESS_MALLOC_WARN
#include "../arena.h"

int main(void)
{

    Arena *arena = arena_create(1024); // Allocate a 1kB arena

    char *first_half = arena_alloc(arena, 7);
    memcpy(first_half, "Hello \0", 7);
    char *second_half = arena_alloc(arena, 7);
    memcpy(second_half, "world!\0", 7);

    printf("%s%s\n", first_half, second_half);
    
    arena_clear(arena); // "free" all memory allocated in an arena.
                        // 2 allocations, 1 "free"

    char *first_half_2 = arena_alloc(arena, 14);
    memcpy(first_half_2, "Numbers 1-3: \0", 14);

    int *second_half_2 = arena_alloc(arena, sizeof(int) * 3);
    int array_to_copy[3] = {1, 2, 3};
    memcpy(second_half_2, array_to_copy, sizeof(int) * 3);

    printf("%s\n", first_half_2);
    for(int i = 0; i < 3; i++)
    {
        printf("%d\n", second_half_2[i]);
    }

    // Notice we don't have to clear here.

    arena_destroy(arena); // Free the allocated arena and everything in it

    return 0;
}