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
```

  After doing that, you can `#include "arena.h"`
  And for any other file in any other translation
  unit in the build, you can `#include "arena.h"`
  like you normally would by itself.
*/

#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

typedef struct
{
    char *region;
    size_t index;
    size_t size;
} Arena;

Arena* arena_create(size_t size);
void* arena_alloc(Arena *arena, size_t size);
void* arena_alloc_aligned(Arena *arena, size_t size, unsigned int alignment);
void arena_clear(Arena* arena);
void arena_destroy(Arena *arena);

#ifdef ARENA_IMPLEMENTATION

#if !defined(ARENA_MALLOC) || !defined(ARENA_FREE)

    #ifndef ARENA_SUPPRESS_MALLOC_WARN
        #warning "Using <stdlib.h> malloc and free, because a replacement for one or both was not specified before including 'arena.h'."
    #endif /* !ARENA_SUPPRESS_MALLOC_WARN */

    #include <stdlib.h>
    #define ARENA_MALLOC malloc
    #define ARENA_FREE free

#endif /* !defined ARENA_MALLOC, ARENA_FREE */

/*
Allocate and return a pointer to memory to the arena
with a region with the specified size.

Parameters:
  size_t size    |    The size (in bytes) of the arena
                      memory region.
Return:
  Pointer to arena on success, NULL on failure
*/
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

    arena->index = 0;
    arena->size = size;
    return arena;
}

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

    arena->index += size;
    return arena->region + (arena->index - size);
}

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

/*
Reset the pointer to the arena region to the beginning
of the allocation. Allows reuse of the memory without
realloc or frees.

Parameters:
  Arena *arena    |    The arena to be cleared.
*/
void arena_clear(Arena *arena)
{
    if(arena == NULL)
    {
        return;
    }
    
    arena->index = 0;
}


/*
Free the memory allocated for the entire arena region.

Parameters:
  Arena *arena    |    The arena to be destroyed.
*/
void arena_destroy(Arena *arena)
{
    if(arena == NULL)
    {
        return;
    }
    
    if(arena->region != NULL)
    {
        ARENA_FREE(arena->region);            
    }

    ARENA_FREE(arena);
}

#endif /* ARENA_IMPLEMENTATION */

#endif /* ARENA_H */
