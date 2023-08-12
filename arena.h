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
#warning \
"Using <stdlib.h> malloc and free, because a replacement for one or both \
was not specified before including 'arena.h'."
#endif /* !ARENA_SUPPRESS_MALLOC_WARN */

#include <stdlib.h>
#define ARENA_MALLOC malloc
#define ARENA_FREE free

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
        return NULL;
    }

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

    arena->index += size;
    return arena->region + (arena->index - size);
}

void* arena_alloc_aligned(Arena *arena, size_t size, unsigned int alignment)
{
    size_t offset;
    register void* alloc;
    
    if(arena == NULL)
    {
        return NULL;
    }

    if(arena->region == NULL)
    {
        return NULL;
    }

    offset = arena->index % alignment;
    if(arena->size - (arena->index + offset) < size)
    {
        return NULL;
    }

    alloc = arena->region + arena->index;
    arena->index += size;
    return alloc;
}

void arena_clear(Arena* arena)
{
    if(arena == NULL)
    {
        return;
    }
    arena->index = 0;
}

void arena_destroy(Arena *arena)
{
    if(arena == NULL)
    {
        return;
    }
    
    if(arena->region != NULL)
    {
        free(arena->region);            
    }
    free(arena);
}

#endif /* ARENA_IMPLEMENTATION */

#endif /* ARENA_H */