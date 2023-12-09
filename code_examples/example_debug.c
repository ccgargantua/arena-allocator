#include <stdio.h>  // printf
#include <string.h> // memcpy

#define ARENA_DEBUG
#define ARENA_IMPLEMENTATION
#define ARENA_SUPPRESS_MALLOC_WARN
#include "../arena.h"

int main(void)
{

    Arena *arena = arena_create(1024);

    char *x = arena_alloc(arena, 5);
    char *y = arena_alloc(arena, 25);

    Arena_Allocation *x_allocation = arena_get_allocation_struct(arena, x);
    Arena_Allocation *y_allocation = arena_get_allocation_struct(arena, y);

    printf("X index in region: %ld\n", x_allocation->index);
    printf("X size in region: %ld\n", x_allocation->size);

    printf("Y index in region: %ld\n", y_allocation->index);
    printf("Y size in region: %ld\n", y_allocation->size);

    arena_destroy(arena);

    return 0;
}