# Arena Allocator
Single-header arena allocator. C89 Compatible.

---

## Table of Contents

1. **[About](#about)**
  * 1.1 [Arena Allocators](#arena-allocators)
  * 1.2 [Single-Header Libraries](#single-header-libraries)
  * 1.3 [Disclaimer](#disclaimer)
2. **[Usage](#usage)**
  * 2.1 [Including](#including)
  * 2.2 [Functions](#functions)
3. **[Compatibility](#compatibility)**
  * 3.1 [Compilers](#compilers)
  * 3.2 [Operating Systems](#operating-systems)
4. **[Contributing](#contributing)**
  * 4.1 [Basic Guidelines](#basic-guidelines)
  * 4.2 [Modifications to `arena.h`](#modifications-to-arenah)
  * 4.3 [Testing](#testing)
  * 4.4 [Code Style](#code-style)

---

## About

### Arena Allocators

Arena allocators are a simple way to achieve easier, faster, and safer dynamic memory management by allowing multiple allocations to be freed as a group. This is done by allocating memory in large regions and then distributing portions of that memory as needed, reducing the amount of `malloc` calls (which are slow compared to simple pointer arithmetic).

When you destroy the arena you also free it and all of its contents, reducing the amount of `free` calls which are also slow. Going further, you can clear arenas by simply resetting their memory pointers to `0`, allowing you to reuse them and eliminating the need for even more `malloc`'s and `free`'s.

You can learn more about arena/zone/region allocators by reading this [fantastic article](https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator).

### Single-Header Libraries

Whenever I share this project with other programmers, one of the most common responses I receive is something along the lines of: *You should NEVER put implementation/logic code in a header file!* I take issue with this statement for three reasons...

1. It shows that an outdated and, by consequence, harmful construct is still being enforced in the education system, which is where said construct is usually introduced.

2. Very rarely does the person making this statement have an actual reason for believing it. Does this person ever think about *why* they should "NEVER put implementation/logic code in a header file"? Simply regurgitating what they've heard without any basis for *why* they chose to agree with it does not help me in any way, and this person should not expect me to just accept it as they have done.

3. The largest and most valid criticism of header-only/single-header libraries is that a change to the header requires re-compilation of all files that include it. In the case of my project, making changes to `arena.h`, even though the actual implementation is only contained in the translation unit that `#define`'s the `ARENA_IMPLEMENTATION` macro, will result in the rebuild of all files that include it. The solution? Once `arena.h` is in the desired state, stop making changes!

Linking is sluggish and complicated. Many beginners often times struggle with learning the linking process, and they are also the biggest culprit when it comes to writing unsafe code. This alone is enough reason for me to make this arena allocator a header-only library. My code (in its current state) is very small, roughly 200 lines. Why would I make you build and link such a small implementation when you could simply `#include` it once and start using it out of the box? If you really have a problem with it, this allocator in the single-header format does not prevent you from following the source+header construct if you so desire. Heck, feel free to fork it and make it source+header, it's open source for a reason!

### Disclaimer
This does not implement a kernel-level allocator, but instead wraps `malloc` and `free` (standard library or custom, your choice).

---

## Usage
Documentation for the arena allocator can all be found in `arena.h`. There is a comment at the top of the header file with quick instructions for usage for your convenience.

### Including
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

After doing this in **one** file in **one** translation unit, for **any other file** you can include normally with a lone `#include "arena.h"`. You can find usage examples in the [`code_examples/` folder](https://github.com/ccgargantua/arena-allocator/tree/main/code_examples).

### Types

There are two structs defined in `arena.h`. This lists each one along with its members.

* **`Arena_Allocation`** The data structure for an arena allocation. Available only when `ARENA_DEBUG` is defined.
  * `size_t index` The index in the arena in which the beginning of the allocation is located.
  * `size_t size` The size of the memory allocated to this allocation in bytes.
  * `char *pointer` The pointer associated for the allocation.
  * `struct Arena_Allocation_s *next` The next allocation in the linked list.


* **`Arena`** The data structure for an arena.
  * `char *region` The region of allocated memory.
  * `size_t index` The index of the region for the next pointer to be distributed.
  * `size_t size` The size of memory allocated to the arena in bytes.
  * `unsigned long allocations` The number of arena allocations that have been made. Only available when `ARENA_DEBUG` is defined.
  * `Arena_Allocation *head_allocation` The first allocation made in the arena (used for a linked list). Only available when `ARENA_DEBUG` is defined.


### Functions
There is also documentation for each function in the form of comments above each forward declaration. Below is an almost direct copy-paste from `arena.h`.
```c
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
will get some hard-to-track bugs. Providing a size of
zero results in a failure.

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
*/
ARENA_INLINE void arena_copy(Arena *dest, Arena *src);


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
region. This function is only available when ARENA_DEBUG
is defined.

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
```

---

## Compatibility

The code was written to build with any compiler and run on any platform that supports the C89 standard. However, testing has been somewhat limited and there are some issues with building on Windows with the `Makefile`. Please read all of the below information.

### Compilers

The tests and examples have been compiled and successfully run under the following compilers and versions:

* Clang 17
* GCC 13.2
* MSVC 17.8

### Operating Systems

The tests and examples were compiled and successfully run on the following operating systems:

* Ubuntu 20.04
* Windows 10

**NOTE** The Makefile currently has not been configured to work on Windows when running `$ make test` due to the use of valgrind and the absense of `.exe` extensions. This should be a simple fix and is on my TODO list (feel free to open an issue and fix this yourself!).

---

## Contributing

### Basic Guidelines

This project has very simple guidelines for contributing.

 * For any **contributions to the code of `arena.h`**, please open a pull request only if you are **addressing an issue** to fulfill a **feature request or fix a bug**. Follow the code style and run the automated tests before opening a pull request. All tests must be passed.

 * For **feature requests** or **bugs**, please **open an issue**. You can then address this issue yourself.

 * For any **anything else**, open an issue and we will discuss it.

### Modifications to `arena.h`

At the moment there is no documentation for the code style, but it should be relatively simple enough to pick up on through reading existing code for most things. If you are having trouble, feel free to open an issue for a FR. If it already exists, comment on it describing what you are confused by.

* If you modify `arena.h` whatsoever, you must run the tests. See the next section.

* If you add a feature within `arena.h`, you must create an adequate test or tests within `test.c`.

* If you add a feature within `arena.h`, you must *should* an adequate example in `code_examples/` **and** add it to the `makefile`, but it is not required.

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

### Code Style

* **Identifiers**
  * **Variables** should be lowercase and snake case, eg `variable_name`. For **pointers**, the asterisk (`*`) should be attached to the variable name, not the type, eg `type *variable_name`.
  * **Functions**, like variables, should be lowercase and snake case, eg `type function_name(p1, p2, ...)`. In the case of pointers, the asterisk (`*`) should be attached to the type for density, eg `type* function_name(p1, p2, ...)`. Functions should have a forward declaration with a comment for documentation above it around the top of the file, make sure the order relative to other functions is consistent.
  * **Struct, enum, and union** identifiers should be pascal case, eg `struct StructName`. They should be typedef'd and located around the top of the header file.
  * **Macros** should be all caps and snakecase, beginning with `ARENA_`, eg `ARENA_MACRO_NAME`. **Please avoid adding macros without consulting me first**. Feel free to make changes to existing macros, though.

* **White space** is based on relevance of a line of code to those around it. If you don't understand what these points mean, please look at the code. It should be formatted as follows:
  * A single newline separating closely related code.
  * Two newlines separating unrelated code within the same scope or tag type.
  * Three newlines separating code within different function scopes, tag types, and blocks of preprocessor directives.

* **Error checking** should be done whenever possible and mimic the behavior of standard library implementations, such as returning `NULL` on error in functions that return pointers or returning integer error values from integer functions. You should use **early error checking**, which means checking for errors as soon as they could be produced, eg. checking for a `NULL` returned after a failed `malloc` call.

* **Comments** should describe *why* you did something, not *what* it is that you did. In other words, your code should be self-explanatory. Documentation for functions in the form of comments should be located above the function in the following format:

```c
/*
Description of function, description of function description of function.
Description of function description of function, description of function
description of function.

Parameters:
  paramter1_type paramter1_name    |    Description of parameter 1, description
                                        of parameter one description of paramter
                                        1.
  paramter2_type paramter2_name    |    Description of parameter 2, description
                                        of parameter one description of paramter
                                        2.
Return:
  Description of return value, description of return value description of return value.
*/
```