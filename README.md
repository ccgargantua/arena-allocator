# Arena Allocator
Single-header arena allocator. C89 Compatible.

---

## About
Arena allocators are a simple way to achieve easier, faster and safer dynamic memory management by allowing multiple allocations to be freed as a group. This is done by allocating memory in large groups and then allocating portions of that memory as needed, reducing the amount of `malloc` calls (which are slow compared to adding and subtracting from a pointer).

When you destroy the arena you also free it and all of its contents, reducing the amount of `free` calls which are also slow. Going further, you can clear arenas by simply resetting their memory pointers to `0`, allowing you to reuse them and eliminating the need for even more `malloc`'s and `free`'s.

You can learn more about arena/zone/region allocators [here](https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator).

### Disclaimer
This is not a full-blown allocator, at its core it is a wrapper for malloc and free (stdlib or your own).

---

## Usage
There is a comment at the top of the header file with quick instructions for usage for your convenience. This will go a little more in-depth.

For one file in one translation unit, you need to define some macros before including `arena.h`. The two optional macros `ARENA_MALLOC` and `ARENA_FREE` can be assigned to alternative `malloc`-like and `free`-like allocators and deallocators respectively.
```c
#define ARENA_IMPLEMENTATION

// All of these are optional
#define ARENA_MALLOC <stdlib_malloc_like_allocator>
#define ARENA_FREE <stdlib_free_like_deallocator>
#define ARENA_MEMCPY <stdlib_memcpy_like_copier>

// for debug functionality, you can also do:
#define ARENA_DEBUG
```

After doing this in **one** file in **one** translation unit, for **any other file** you can include normally with a lone `#include "arena.h"`.

### Example
`example.c`
```c
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
```
**Build and run**
```
$ gcc -o example code_examples/example.c

$ ./example
> Hello world!
  Numbers 1-3: 
  1
  2
  3

$ valgrind ./example
> ==7448== Memcheck, a memory error detector
  ==7448== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
  ==7448== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
  ==7448== Command: ./example
  ==7448== 
  Hello world!
  Numbers 1-3: 
  1
  2
  3
  ==7448== 
  ==7448== HEAP SUMMARY:
  ==7448==     in use at exit: 0 bytes in 0 blocks
  ==7448==   total heap usage: 3 allocs, 3 frees, 2,072 bytes allocated
  ==7448== 
  ==7448== All heap blocks were freed -- no leaks are possible
  ==7448== 
  ==7448== For lists of detected and suppressed errors, rerun with: -s
  ==7448== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

### Alignment
There is also support for alignment.

`example_aligned.c`
```c
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
```
**Build and run**
```
$ gcc -o example_aligned code_examples/example_aligned.c

$ ./example_aligned
> 10
  22
  34

$ valgrind ./example_aligned
> ==7457== Memcheck, a memory error detector
  ==7457== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
  ==7457== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
  ==7457== Command: ./example_aligned
  ==7457== 
  10
  22
  34
  ==7457== 
  ==7457== HEAP SUMMARY:
  ==7457==     in use at exit: 0 bytes in 0 blocks
  ==7457==   total heap usage: 3 allocs, 3 frees, 2,072 bytes allocated
  ==7457== 
  ==7457== All heap blocks were freed -- no leaks are possible
  ==7457== 
  ==7457== For lists of detected and suppressed errors, rerun with: -s
  ==7457== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

```

### Debug

You can also define the `ARENA_DEBUG` macro before including `arena.h`, which will generate linked-list code to track allocations and their sizes.

```c
#define ARENA_DEBUG
#define ARENA_IMPLEMENTATION
#define ARENA_SUPPRESS_MALLOC_WARN
#include "arena.h"
```

This generates a nodal `Arena_Allocation` struct with the members `size` (indicating the size of the allocation) and a pointer to the next `Arena_Allocation` in the linked list, `next`. It adds two additional members to the `Arena` struct, `allocations` which is an integer containing the number of allocations made, and `head_allocation`, which is the first `Arena_Allocation` pointer node in the linked list of allocations.

This debug information can be accessed by using the `arena_get_allocation_struct` method which, when supplied with an arena and a pointer to memory allocated from that arena, will return a pointer to the `Arena_Allocation` struct that is associated with that pointer (or NULL on failure).

```c
#include <stdio.h>  // printf
#include <string.h> // memcpy

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
```
**Build and run**
```
$ gcc -o -g example_debug code_examples/example_debug.c

$ ./example_debug 
> X index in region: 0
  X size in region: 5
  Y index in region: 5
  Y size in region: 25 
```

---

## Contributing

This project has very simple guidelines for contributing.

 * For any **contributions to the code of `arena.h`**, please open a pull request only if you are **addressing an issue** to fulfill a **feature request or fix a bug**. Follow the code style and run the automated tests before opening a pull request. All tests must be passed.

 * For **feature requests** or **bugs**, please **open an issue**.

 * For any **anything else**, open an issue and we will discuss it.

### Modifications to `arena.h`

At the moment there is no documentation for the code style, but it should be relatively simple enough to pick up on through reading existing code for most things. If you are having trouble, feel free to open an issue for a FR. If it already exists, comment on it describing what you are confused by.

* If you modify `arena.h` whatsoever, you must run the tests. See the next section.

* If you add a feature within `arena.h`, you must create an adequate test or tests.

* If you add a feature within `arena.h`, you must create an adequate example in `code_examples/` **and** add it to the `makefile`.

### Testing

To test the code, build and run the tests with:

```
$ make tests
$ ./test
```

If you change `arena.h` whatsoever, **run the tests before opening a PR**. If you open a PR with modifictions to the code and the tests don't all pass, make a comment on your PR stating which test you believe is wrong and is preventing you from passing all of the tests. If any test fails and your PR doesn't have a comment that claims to correct a failed test, your PR will be ignored.

Outside of addressing bugs and feature requests, fulfilling a feature request or bug fix for functionality within `arena.h` permits modifying or adding relevant testing code within `test.c`, and you must do so if you want your PR to be acknowledged. There is documentation for testing code within `test.c` at the top of the file in the form of comments.

The tests must also pass through valgrind leak-free.

```
$ valgrind ./test
```

You can build the tests and run them under valgrind in one command with:

```
$ make test
```