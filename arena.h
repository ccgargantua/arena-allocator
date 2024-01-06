/*
See README.md for more in-depth usage docs and an
example.

QUICK USAGE:
  One file in one translation unit must have the
  following before including "arena.h", replacing
  the macro values appropriately when needed.

```
#define ARENA_IMPLEMENTATION

// Either both of these...
#define ARENA_MALLOC <stdlib_malloc_like_allocator>
#define ARENA_FREE <stdlib_free_like_deallocator>
// ... or just this
#define ARENA_SUPPRESS_MALLOC_WARN // alternatively using compiler flag -D with same name

// for debug functionality, you can also do:
#define ARENA_DEBUG
```

  After doing that, you can `#include "arena.h"`
  And for any other file in any other translation
  unit in the build, you can `#include "arena.h"`
  like you normally would by itself.
*/


#ifndef ARENA_H
#define ARENA_H


#include <stddef.h>


#if __STDC_VERSION__ >= 199901L
    #define ARENA_INLINE inline
#else
    #define ARENA_INLINE
#endif /* __STDC_VERSION__ >= 199901L */


#ifdef ARENA_DEBUG

/* We are debugging this arena allocator, not your implementation of malloc/free */
#include <stdlib.h>

typedef struct Arena_Allocation_s
{
    size_t index;
    size_t size;
    char *pointer;
    struct Arena_Allocation_s *next;
} Arena_Allocation;

#endif /* ARENA_DEBUG */


typedef struct
{
    char *region;
    size_t index;
    size_t size;

    #ifdef ARENA_DEBUG
    unsigned long allocations;
    Arena_Allocation *head_allocation;
    #endif /* ARENA_DEBUG */
} Arena;


/*
Allocate and return a pointer to memory to the arena
with a region with the specified size.

Parameters:
  size_t size    |    The size (in bytes) of the arena
                      memory region.
Return:
  Pointer to arena on success, NULL on failure
*/
Arena* arena_create(size_t size);


/*
Return a pointer to a portion of specified size of the
specified arena's region. Nothing will restrict you
from allocating more memory than you specified, so be
mindful of your memory (as you should anyways) or you
will get some hard-to-track bugs.

Parameters:
  Arena *arena    |    The arena of which the pointer
                       from the region will be
                       distributed
  size_t size     |    The size (in bytes) of
                       allocated memory planned to be
                       used.
Return:
  Pointer to arena region segment on success, NULL on
  failure.
*/
void* arena_alloc(Arena *arena, size_t size);


/*
Same as arena_alloc, except you can specify a
memory alignment for allocations.

Return a pointer to a portion of specified size of the
specified arena's region. Nothing will restrict you
from allocating more memory than you specified, so be
mindful of your memory (as you should anyways) or you
will get some hard-to-track bugs.

Parameters:
  Arena *arena              |    The arena of which the pointer
                                 from the region will be
                                 distributed
  size_t size               |    The size (in bytes) of
                                 allocated memory planned to be
                                 used.
  unsigned int alignment    |    Alignment (in bytes) for each
                                 memory allocation.
Return:
  Pointer to arena region segment on success, NULL on
  failure.
*/
void* arena_alloc_aligned(Arena *arena, size_t size, unsigned int alignment);


/*
Copy the memory contents of one arena to another.

Parameters:
  Arena *src     |    The arena being copied, the source.
  Arena *dest    |    The arena being copied to. Must be created/allocated
                      already.
*/
ARENA_INLINE void arena_copy(Arena *src, Arena *dest);


/*
Reset the pointer to the arena region to the beginning
of the allocation. Allows reuse of the memory without
realloc or frees.

Parameters:
  Arena *arena    |    The arena to be cleared.
*/
void arena_clear(Arena* arena);


/*
Free the memory allocated for the entire arena region.

Parameters:
  Arena *arena    |    The arena to be destroyed.
*/
void arena_destroy(Arena *arena);


/*
Returns a pointer to the allocation struct associated
with a pointer to a segment in the specified arena's
region.

Parameters:
  Arena *arena    |    The arena whose region should
                       have a portion pointed to by
                       ptr.
  void *ptr       |    The ptr being searched for
                       within the arena in order to
                       find an allocation struct
                       associated with it.
*/
#ifdef ARENA_DEBUG
Arena_Allocation* arena_get_allocation_struct(Arena *arena, void *ptr);
#endif /* ARENA_DEBUG */


#ifdef ARENA_IMPLEMENTATION


#if !defined(ARENA_MALLOC) || !defined(ARENA_FREE) || !defined(ARENA_MEMCPY)

    #ifndef ARENA_SUPPRESS_MALLOC_WARN
        #warning "Using <stdlib.h> malloc and free, because a replacement for one or both was not specified before including 'arena.h'."
    #endif /* !ARENA_SUPPRESS_MALLOC_WARN */

    #include <stdlib.h>
    #define ARENA_MALLOC malloc
    #define ARENA_FREE free

    #include <string.h>
    #define ARENA_MEMCPY memcpy

#endif /* !defined ARENA_MALLOC, ARENA_FREE */


Arena* arena_create(size_t size)
{
    Arena *arena = ARENA_MALLOC(sizeof(Arena));
    if(arena == NULL)
    {
        return NULL;
    }

    arena->region = ARENA_MALLOC(size);
    if(arena->region == NULL)
    {
        ARENA_FREE(arena);
        return NULL;
    }

    #ifdef ARENA_DEBUG
    arena->head_allocation = NULL;
    arena->allocations = 0;
    #endif /* ARENA_DEBUG */

    arena->index = 0;
    arena->size = size;
    return arena;
}


void* arena_alloc(Arena *arena, size_t size)
{
    if(arena == NULL)
    {
        return NULL;
    }

    if(arena->region == NULL)
    {
        return NULL;
    }

    if(arena->size - arena->index < size)
    {
        return NULL;
    }

    #ifdef ARENA_DEBUG

    if(arena->head_allocation == NULL)
    {
        arena->head_allocation = malloc(sizeof(Arena_Allocation));
        arena->head_allocation->index = arena->index;
        arena->head_allocation->size = size;
        arena->head_allocation->pointer = arena->region + arena->index;
        arena->head_allocation->next = NULL;
    }
    else
    {
        Arena_Allocation *current = arena->head_allocation;
        while(current->next != NULL)
        {
            current = current->next;
        }

        current->next = malloc(sizeof(Arena_Allocation));
        current->next->index = arena->index;
        current->next->size = size;
        current->next->pointer = arena->region + arena->index;
        current->next->next = NULL;
    }

    arena->allocations++;

    #endif /* ARENA_DEBUG */

    arena->index += size;
    return arena->region + (arena->index - size);
}


void* arena_alloc_aligned(Arena *arena, size_t size, unsigned int alignment)
{
    unsigned int offset;

    if(arena == NULL)
    {
        return NULL;
    }

    if(arena->region == NULL)
    {
        return NULL;
    }

    offset = (size_t)(arena->region + arena->index) % alignment;
    if(arena->size - (arena->index + offset) < size)
    {
        return NULL;
    }

    if(offset > 0)
    {
        arena->index = arena->index - offset + alignment;
    }

    return arena_alloc(arena, size);
}


ARENA_INLINE void arena_copy(Arena *src, Arena *dest)
{
    ARENA_MEMCPY(dest->region, src->region, src->index);
}


void arena_clear(Arena *arena)
{
    if(arena == NULL)
    {
        return;
    }

    #ifdef ARENA_DEBUG

    while(arena->head_allocation != NULL)
    {
        Arena_Allocation *next = arena->head_allocation->next;
        free(arena->head_allocation);
        arena->head_allocation = next;
    }
    arena->allocations = 0;
    arena->head_allocation = NULL;

    #endif /* ARENA_DEBUG */

    arena->index = 0;
}


void arena_destroy(Arena *arena)
{
    if(arena == NULL)
    {
        return;
    }

    #ifdef ARENA_DEBUG
    arena_clear(arena);
    #endif /* ARENA_DEBUG */

    if(arena->region != NULL)
    {
        ARENA_FREE(arena->region);
    }

    ARENA_FREE(arena);
}


#ifdef ARENA_DEBUG

Arena_Allocation* arena_get_allocation_struct(Arena *arena, void *ptr)
{
    Arena_Allocation *current = arena->head_allocation;
    while(current != NULL)
    {
        if(current->pointer == (char *)ptr)
        {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

#endif /* ARENA_DEBUG */


#endif /* ARENA_IMPLEMENTATION */


#endif /* ARENA_H */
