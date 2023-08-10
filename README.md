# arena-allocator
Single-header arena allocator written in ANSI C.

---

## About
Small and single-header arena allocator, written in ANSI C. I made this for personal use, and will be refining it as time goes on and as needed.

---

## Usage
There is a comment at the top of the header file with quick instructions for usage for your convenience. This will go a little more in-depth.

For one file in one translation unit, you need to define some macros before including `arena.h`.
```c
#define ARENA_IMPLEMENTATION

// You will get a warning if you don't specify:
// either both of these...
#define ARENA_MALLOC <stdlib_malloc_like_allocator> // defaults to stdlib malloc
#define ARENA_FREE <stdlib_free_like_deallocator>   // defaults to stdlib free
// ... or just this
#define ARENA_SUPPRESS_MALLOC_WARN // Alternatively use compiler flag -DARENA_SUPPRESS_MALLOC_WARN

#include "arena.h"
```

After doing this in **one** file in **one** translation unit, for **every other file** you can include normally with a lone `#include "arena.h"`.

### Example
`code.c`
```c
#include <stdio.h>
#include <string.h>

#define ARENA_IMPLEMENTATION
#include "arena.h"

int main(void)
{

    Arena *arena = arena_create(1024); // Allocate a 1kB arena
    if(arena == NULL)
    {
        return 1;
    }

    char *first_half = arena_alloc(7);
    if(first_half == NULL)
    {
        return 1;
    }
    memcpy(first_half, "Hello \0");

    char *second_half = arena_alloc(7);
    if(second_half == NULL)
    {
        return 1;
    }
    memcpy(second_half, "world!\0");

    printf("%s%s\n", first_half, second_half);

    arena_destroy(arena);

    return 0;
}
```
**Build**
```
$ gcc -DARENA_SUPPRESS_MALLOC_WARN -o example code.c
$ ./example
> Hello world!
  Numbers 1-3: 
  1
  2
  3

$ gcc -o example code.c
> In file included from code.c:5:
  ../arena.h:45:2: warning: #warning "Using <stdlib.h> malloc and free, because a replacement for one or both was not specified before including 'arena.h'." [-Wcpp]
     45 | #warning \
        |  ^~~~~~~

$ ./example
> Hello world!
  Numbers 1-3: 
  1
  2
  3

$ valgrind ./example
> ==27576== Memcheck, a memory error detector
  ==27576== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
  ==27576== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
  ==27576== Command: ./example
  ==27576== 
  Hello world!
  Numbers 1-3: 
  1
  2
  3
  ==27576== 
  ==27576== HEAP SUMMARY:
  ==27576==     in use at exit: 0 bytes in 0 blocks
  ==27576==   total heap usage: 3 allocs, 3 frees, 2,072 bytes allocated
  ==27576== 
  ==27576== All heap blocks were freed -- no leaks are possible
  ==27576== 
  ==27576== For lists of detected and suppressed errors, rerun with: -s
  ==27576== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

```

---

## Contributing
Feel free to make suggestions by opening an issue. I will not accept any PRs.