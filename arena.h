/*
See README.md for more in-depth usage docs and an
example.

QUICK USAGE:
  One file in one translation unit must have the
  following before including "arena.h", replacing
  the macro values appropriately when desired.

```
#define ARENA_IMPLEMENTATION

// All of these are optional
#define ARENA_MALLOC <stdlib_malloc_like_allocator>
#define ARENA_FREE <stdlib_free_like_deallocator>
#define ARENA_MEMCPY <stdlib_memcpy_like_copier>

// for debug functionality, you can also do:
#define ARENA_DEBUG

// If you would like to change the default alignment for
// allocations, you can define:
#define ARENA_DEFAULT_ALIGNMENT <alignment_value>
```

  After doing that, you can `#include "arena.h"`
  And for any other file in any other translation
  unit in the build, you can `#include "arena.h"`
  like you normally would by itself.
*/


#ifndef ARENA_H
#define ARENA_H


#include <stddef.h>


#if __STDC_VERSION__ >= 199901L && !defined(ARENA_DEBUG)
    #define ARENA_INLINE extern inline
#else
    #define ARENA_INLINE
#endif /* __STDC_VERSION__ >= 199901L */


#if __STDC_VERSION__ >= 201112L
    #include <stdalign.h>
    #define ARENA_ALIGNOF(type) alignof(type)
#else
    #define ARENA_ALIGNOF(type) offsetof(struct { char c; type d; }, d)
#endif


#ifndef ARENA_DEFAULT_ALIGNMENT
    #define ARENA_DEFAULT_ALIGNMENT ARENA_ALIGNOF(size_t)
#endif


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
with a region with the specified size. Providing a
size of zero results in a failure.

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
will get some hard-to-track bugs. By default, memory is
aligned by alignof(size_t), but you can change this by
#defining ARENA_DEFAULT_ALIGNMENT before #include'ing
arena.h. Providing a size of zero results in a failure.

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
ARENA_INLINE void* arena_alloc(Arena *arena, size_t size);


/*
Same as arena_alloc, except you can specify a memory
alignment for allocations.

Return a pointer to a portion of specified size of the
specified arena's region. Nothing will restrict you
from allocating more memory than you specified, so be
mindful of your memory (as you should anyways) or you
will get some hard-to-track bugs. Providing a size of
zero results in a failure.

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

Return:
  Number of bytes copied.
*/
ARENA_INLINE size_t arena_copy(Arena *dest, Arena *src);


/*
Reset the pointer to the arena region to the beginning
of the allocation. Allows reuse of the memory without
realloc or frees.

Parameters:
  Arena *arena    |    The arena to be cleared.
*/
ARENA_INLINE void arena_clear(Arena* arena);


/*
Free the memory allocated for the entire arena region.

Parameters:
  Arena *arena    |    The arena to be destroyed.
*/
ARENA_INLINE void arena_destroy(Arena *arena);


#ifdef ARENA_DEBUG

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
Arena_Allocation* arena_get_allocation_struct(Arena *arena, void *ptr);


/*
Adds an arena allocation to the arena's linked list of
allocations under debug.

Parameters:
  Arena *arena    |    The arena whose allocation list
                       should be added to
  size_t size     |    The size of the allocation being
                       added.
*/
void arena_add_allocation(Arena *arena, size_t size);


/*
Deletes the arena's linked list of allocations under
debug.

Parameters:
  Arena *arena    |    The arena whose allocation list
                       is being deleted.
*/
void arena_delete_allocation_list(Arena *arena);

#endif /* ARENA_DEBUG */


#ifdef ARENA_IMPLEMENTATION


#ifndef ARENA_MALLOC
    #include <stdlib.h>
    #define ARENA_MALLOC malloc
#endif /* !ARENA_MALLOC */

#ifndef ARENA_FREE
    #include <stdlib.h>
    #define ARENA_FREE free
#endif /* !ARENA_FREE */

#ifndef ARENA_MEMCPY
    #include <string.h>
    #define ARENA_MEMCPY memcpy
#endif /* !ARENA_MEMCPY */


Arena* arena_create(size_t size)
{
    Arena *arena;

    if(size == 0)
    {
        return NULL;
    }

    /* A malloc of 0 is implementation defined, so the above check is necessary */
    arena = ARENA_MALLOC(sizeof(Arena));
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

    arena->index = 0;
    arena->size = size;

    #ifdef ARENA_DEBUG
    arena->head_allocation = NULL;
    arena->allocations = 0;
    #endif /* ARENA_DEBUG */

    return arena;
}


ARENA_INLINE void* arena_alloc(Arena *arena, size_t size)
{
    return arena_alloc_aligned(arena, size, ARENA_DEFAULT_ALIGNMENT);
}


void* arena_alloc_aligned(Arena *arena, size_t size, unsigned int alignment)
{
    unsigned int offset;

    if(size == 0)
    {
        return NULL;
    }

    if(arena == NULL || arena->region == NULL)
    {
        return NULL;
    }

    if(alignment != 0)
    {
        offset = (size_t)(arena->region + arena->index) % alignment;
        if(offset > 0)
        {
            arena->index = arena->index - offset + alignment;
        }
    }
    else
    {
        offset = 0;
    }

    if(arena->size - (arena->index + offset) < size)
    {
        return NULL;
    }

    #ifdef ARENA_DEBUG
    arena_add_allocation(arena, size);
    #endif /* ARENA_DEBUG */

    arena->index += size;
    return arena->region + (arena->index - size);
}


ARENA_INLINE size_t arena_copy(Arena *dest, Arena *src)
{
    size_t bytes;

    if(dest == NULL || src == NULL)
    {
        return 0;
    }

    if(src->index < dest->size)
    {
        bytes = src->index;
    }
    else
    {
        bytes = dest->size;
    }

    ARENA_MEMCPY(dest->region, src->region, bytes);
    dest->index = bytes;

    return bytes;
}


ARENA_INLINE void arena_clear(Arena *arena)
{
    if(arena == NULL)
    {
        return;
    }

    arena->index = 0;

    #ifdef ARENA_DEBUG
    arena_delete_allocation_list(arena);
    #endif /* ARENA_DEBUG */
}


ARENA_INLINE void arena_destroy(Arena *arena)
{
    if(arena == NULL)
    {
        return;
    }

    #ifdef ARENA_DEBUG
    arena_delete_allocation_list(arena);
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
    Arena_Allocation *current;

    if(arena == NULL || ptr == NULL)
    {
        return NULL;
    }

    current = arena->head_allocation;
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


void arena_add_allocation(Arena *arena, size_t size)
{
    if(arena == NULL)
    {
        return;
    }

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
}


void arena_delete_allocation_list(Arena *arena)
{
    if(arena == NULL)
    {
        return;
    }

    while(arena->head_allocation != NULL)
    {
        Arena_Allocation *next = arena->head_allocation->next;
        free(arena->head_allocation);
        arena->head_allocation = next;
    }

    arena->allocations = 0;
    arena->head_allocation = NULL;
}

#endif /* ARENA_DEBUG */


#endif /* ARENA_IMPLEMENTATION */


#endif /* ARENA_H */
