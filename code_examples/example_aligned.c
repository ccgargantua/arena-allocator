#include <stdio.h>  // printf

#define ARENA_IMPLEMENTATION
#define ARENA_SUPPRESS_MALLOC_WARN
#include "../arena.h"

int main(void)
{
    Arena *arena = arena_create(1024); // Allocate a 1kB arena

    char *char_ptr_1 = arena_alloc_aligned(arena, 10, 4);
    printf("%ld\n", arena->index); // 10    

    char *char_ptr_2 = arena_alloc_aligned(arena, 10, 4);
    printf("%ld\n", arena->index); // 22    

    char *char_ptr_3 = arena_alloc_aligned(arena, 10, 4);
    printf("%ld\n", arena->index); // 34

    arena_destroy(arena); // Free the allocated arena and everything in it

    return 0;
}