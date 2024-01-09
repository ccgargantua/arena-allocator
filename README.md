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

After doing this in **one** file in **one** translation unit, for **any other file** you can include normally with a lone `#include "arena.h"`. You can find usage examples in the [`code_examples/` folder](https://github.com/ccgargantua/arena-allocator/tree/main/code_examples).

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