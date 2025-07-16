# Arena Allocator
Single-header arena allocator. C89 Compatible.

---

## Table of Contents

1. **[About](#about)**
  * 1.1 [Arena Allocators](#arena-allocators)
  * 1.2 [C89 Compliance](#c89-compliance)
  * 1.3 [Single-Header Libraries](#single-header-libraries)
  * 1.4 [Disclaimer](#disclaimer)
  * 1.5 [LICENSE](#LICENSE)
2. **[Usage](#usage)**
  * 2.1 [Including](#including)
  * 2.2 [Functions and Macros](#functions-and-macros)
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

### C89 Compliance

I'll keep this short. This is not completely C89 compliant due to representing pointers as integers, something that cannot be done if strictly following C89 due to lack of `uintptr_t` I am maintaining C89 compliance for fun, not because I use it. I personally am a C11 enjoyer. If you think C89 is the only way, well, good for you! But you're wrong.

### Single-Header Libraries

Whenever I share this project with other programmers, one of the most common responses I receive is something along the lines of: *You should NEVER put implementation/logic code in a header file!* I take issue with this statement for three reasons...

1. It shows that an outdated and, by consequence, harmful construct is still being enforced in the education system, which is where said construct is usually introduced.

2. Very rarely does the person making this statement have an actual reason for believing it. Does this person ever think about *why* they should "NEVER put implementation/logic code in a header file"? Simply regurgitating what they've heard without any basis for *why* they chose to agree with it does not help me in any way, and this person should not expect me to just accept it as they have done.

3. The largest and most valid criticism of header-only/single-header libraries is that a change to the header requires re-compilation of all files that include it. In the case of my project, making changes to `arena.h`, even though the actual implementation is only contained in the translation unit that `#define`'s the `ARENA_IMPLEMENTATION` macro, will result in the rebuild of all files that include it. The solution? Once `arena.h` is in the desired state, stop making changes!

Linking is sluggish and complicated. Many beginners often times struggle with learning the linking process, and they are also the biggest culprit when it comes to writing unsafe code. This alone is enough reason for me to make this arena allocator a header-only library. My code (in its current state) is very small, roughly 300 lines. Why would I make you build and link such a small implementation when you could simply `#include` it once and start using it out of the box? If you really have a problem with it, this allocator in the single-header format does not prevent you from following the source+header construct if you so desire. Heck, feel free to fork it and make it source+header, it's open source for a reason!

### Disclaimer
This does not implement a kernel-level allocator, but instead wraps `malloc` and `free` (standard library or custom, your choice).


### LICENSE
While I do believe software should be open source, I don't believe it would ethical to require software that uses this library to also be open source. In the modern age of technology and the current state of the world, writing memory-safe code is more important than ever. For this reason, this software is licensed under the Apache License Version 2.0. You are strongly encouraged to read the `LICENSE` (included below, in `arena.h`, and its own file in this repo) if you are considering using the software, unless you believe you are 100% familiar with the terms and conditions.

```
                                 Apache License
                           Version 2.0, January 2004
                        http://www.apache.org/licenses/

   TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION

   1. Definitions.

      "License" shall mean the terms and conditions for use, reproduction,
      and distribution as defined by Sections 1 through 9 of this document.

      "Licensor" shall mean the copyright owner or entity authorized by
      the copyright owner that is granting the License.

      "Legal Entity" shall mean the union of the acting entity and all
      other entities that control, are controlled by, or are under common
      control with that entity. For the purposes of this definition,
      "control" means (i) the power, direct or indirect, to cause the
      direction or management of such entity, whether by contract or
      otherwise, or (ii) ownership of fifty percent (50%) or more of the
      outstanding shares, or (iii) beneficial ownership of such entity.

      "You" (or "Your") shall mean an individual or Legal Entity
      exercising permissions granted by this License.

      "Source" form shall mean the preferred form for making modifications,
      including but not limited to software source code, documentation
      source, and configuration files.

      "Object" form shall mean any form resulting from mechanical
      transformation or translation of a Source form, including but
      not limited to compiled object code, generated documentation,
      and conversions to other media types.

      "Work" shall mean the work of authorship, whether in Source or
      Object form, made available under the License, as indicated by a
      copyright notice that is included in or attached to the work
      (an example is provided in the Appendix below).

      "Derivative Works" shall mean any work, whether in Source or Object
      form, that is based on (or derived from) the Work and for which the
      editorial revisions, annotations, elaborations, or other modifications
      represent, as a whole, an original work of authorship. For the purposes
      of this License, Derivative Works shall not include works that remain
      separable from, or merely link (or bind by name) to the interfaces of,
      the Work and Derivative Works thereof.

      "Contribution" shall mean any work of authorship, including
      the original version of the Work and any modifications or additions
      to that Work or Derivative Works thereof, that is intentionally
      submitted to Licensor for inclusion in the Work by the copyright owner
      or by an individual or Legal Entity authorized to submit on behalf of
      the copyright owner. For the purposes of this definition, "submitted"
      means any form of electronic, verbal, or written communication sent
      to the Licensor or its representatives, including but not limited to
      communication on electronic mailing lists, source code control systems,
      and issue tracking systems that are managed by, or on behalf of, the
      Licensor for the purpose of discussing and improving the Work, but
      excluding communication that is conspicuously marked or otherwise
      designated in writing by the copyright owner as "Not a Contribution."

      "Contributor" shall mean Licensor and any individual or Legal Entity
      on behalf of whom a Contribution has been received by Licensor and
      subsequently incorporated within the Work.

   2. Grant of Copyright License. Subject to the terms and conditions of
      this License, each Contributor hereby grants to You a perpetual,
      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
      copyright license to reproduce, prepare Derivative Works of,
      publicly display, publicly perform, sublicense, and distribute the
      Work and such Derivative Works in Source or Object form.

   3. Grant of Patent License. Subject to the terms and conditions of
      this License, each Contributor hereby grants to You a perpetual,
      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
      (except as stated in this section) patent license to make, have made,
      use, offer to sell, sell, import, and otherwise transfer the Work,
      where such license applies only to those patent claims licensable
      by such Contributor that are necessarily infringed by their
      Contribution(s) alone or by combination of their Contribution(s)
      with the Work to which such Contribution(s) was submitted. If You
      institute patent litigation against any entity (including a
      cross-claim or counterclaim in a lawsuit) alleging that the Work
      or a Contribution incorporated within the Work constitutes direct
      or contributory patent infringement, then any patent licenses
      granted to You under this License for that Work shall terminate
      as of the date such litigation is filed.

   4. Redistribution. You may reproduce and distribute copies of the
      Work or Derivative Works thereof in any medium, with or without
      modifications, and in Source or Object form, provided that You
      meet the following conditions:

      (a) You must give any other recipients of the Work or
          Derivative Works a copy of this License; and

      (b) You must cause any modified files to carry prominent notices
          stating that You changed the files; and

      (c) You must retain, in the Source form of any Derivative Works
          that You distribute, all copyright, patent, trademark, and
          attribution notices from the Source form of the Work,
          excluding those notices that do not pertain to any part of
          the Derivative Works; and

      (d) If the Work includes a "NOTICE" text file as part of its
          distribution, then any Derivative Works that You distribute must
          include a readable copy of the attribution notices contained
          within such NOTICE file, excluding those notices that do not
          pertain to any part of the Derivative Works, in at least one
          of the following places: within a NOTICE text file distributed
          as part of the Derivative Works; within the Source form or
          documentation, if provided along with the Derivative Works; or,
          within a display generated by the Derivative Works, if and
          wherever such third-party notices normally appear. The contents
          of the NOTICE file are for informational purposes only and
          do not modify the License. You may add Your own attribution
          notices within Derivative Works that You distribute, alongside
          or as an addendum to the NOTICE text from the Work, provided
          that such additional attribution notices cannot be construed
          as modifying the License.

      You may add Your own copyright statement to Your modifications and
      may provide additional or different license terms and conditions
      for use, reproduction, or distribution of Your modifications, or
      for any such Derivative Works as a whole, provided Your use,
      reproduction, and distribution of the Work otherwise complies with
      the conditions stated in this License.

   5. Submission of Contributions. Unless You explicitly state otherwise,
      any Contribution intentionally submitted for inclusion in the Work
      by You to the Licensor shall be under the terms and conditions of
      this License, without any additional terms or conditions.
      Notwithstanding the above, nothing herein shall supersede or modify
      the terms of any separate license agreement you may have executed
      with Licensor regarding such Contributions.

   6. Trademarks. This License does not grant permission to use the trade
      names, trademarks, service marks, or product names of the Licensor,
      except as required for reasonable and customary use in describing the
      origin of the Work and reproducing the content of the NOTICE file.

   7. Disclaimer of Warranty. Unless required by applicable law or
      agreed to in writing, Licensor provides the Work (and each
      Contributor provides its Contributions) on an "AS IS" BASIS,
      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
      implied, including, without limitation, any warranties or conditions
      of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A
      PARTICULAR PURPOSE. You are solely responsible for determining the
      appropriateness of using or redistributing the Work and assume any
      risks associated with Your exercise of permissions under this License.

   8. Limitation of Liability. In no event and under no legal theory,
      whether in tort (including negligence), contract, or otherwise,
      unless required by applicable law (such as deliberate and grossly
      negligent acts) or agreed to in writing, shall any Contributor be
      liable to You for damages, including any direct, indirect, special,
      incidental, or consequential damages of any character arising as a
      result of this License or out of the use or inability to use the
      Work (including but not limited to damages for loss of goodwill,
      work stoppage, computer failure or malfunction, or any and all
      other commercial damages or losses), even if such Contributor
      has been advised of the possibility of such damages.

   9. Accepting Warranty or Additional Liability. While redistributing
      the Work or Derivative Works thereof, You may choose to offer,
      and charge a fee for, acceptance of support, warranty, indemnity,
      or other liability obligations and/or rights consistent with this
      License. However, in accepting such obligations, You may act only
      on Your own behalf and on Your sole responsibility, not on behalf
      of any other Contributor, and only if You agree to indemnify,
      defend, and hold each Contributor harmless for any liability
      incurred by, or claims asserted against, such Contributor by reason
      of your accepting any such warranty or additional liability.

   END OF TERMS AND CONDITIONS

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   Copyright 2024 Carter Dugan
```

---

## Usage
Documentation for the arena allocator can all be found in `arena.h`. There is a comment at the top of the header file with quick instructions for usage for your convenience.

### Including
For **one file in one translation unit**, you need the following:
```c
#define ARENA_IMPLEMENTATION
#include "arena.h"
```

From that point, other files in other translation units can simply `#include "arena.h"` normally. There are additional macros you can define/use. See the section on [functions and macros](#functions-and-macros).

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


### Functions and macros
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
```

In your code, you can define some optional macros. `ARENA_MALLOC`, `ARENA_FREE` and `ARENA_MEMCPY` can be assigned to alternative `malloc`-like, `free`-like, and `memcpy`-like functions respectively, and `arena.h` will use them in place of standard library functions. You can access additional debug functionality for keeping track of allocations by defining `ARENA_DEBUG`. Finally, you can also specify a default value for allocation alignment by defining a value for `ARENA_DEFAULT_ALIGNMENT`. See below for examples.

```c
// All of these are optional

// Replace standard library functions
#define ARENA_MALLOC <stdlib_malloc_like_allocator>
#define ARENA_FREE <stdlib_free_like_deallocator>
#define ARENA_MEMCPY <stdlib_memcpy_like_copier>

// for debug functionality:
#define ARENA_DEBUG

// If you would like to change the default alignment for
// allocations:
#define ARENA_DEFAULT_ALIGNMENT <alignment_value>
```

There is also a macro for determining alignment of types. Like everything else, it is also C89-friendly, although when compiling under C11 it will use `stdalign.h`'s `alignof`.

```c
ARENA_ALIGNOF(type) // Gives alignment of `type`
```

---

## Compatibility

The code was written to build with any compiler that supports the C89 standard and run on any platform. However, there are some issues with building on Windows with the `Makefile`. Please read all of the below information.

### Compilers

The tests and examples have been compiled and successfully run under the following compilers and versions:

* Clang 17
* GCC 13.2
* tcc 0.9.27
* MSVC 17.8

### Operating Systems

The tests and examples were compiled and successfully run on the following operating systems:

* Ubuntu 22.04
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

If you change `arena.h` whatsoever, **run the tests before opening a PR**. If you open a PR with modifictions to the code and the tests don't all pass, make a comment on your PR stating which test you believe is wrong and is preventing you from passing all of the tests. If any test fails and your PR doesn't have a comment that claims to correct a failed test, your PR will be ignored closed.

Outside of addressing bugs and feature requests, fulfilling a feature request or bug fix for functionality within `arena.h` permits modifying or adding relevant testing code within `test.c`, and you must do so if you want your PR to be acknowledged. There is documentation for testing code within `test.c` at the top of the file in the form of comments.

The tests must also pass through valgrind leak-free, and `arena.h` **must** be C89 compliant*. You should check this using the `Makefile`, but if for some reason you can't or don't want to, compile `test.c` with

(See note in [C89-Compliance](#c89-compliance))

```
-Werror -Wall -Wextra -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement
```

And for compliance, compile `test_compliance.c` with

```
-pedantic -std=c89 -Werror -Wall -Wextra -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement
```

As I said, you can do all of this with the `Makefile`

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
