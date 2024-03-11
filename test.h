#ifndef TEST_H
#define TEST_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void test_arena_create(void);
void test_arena_alloc(void);
void test_arena_alloc_aligned(void);
void test_arena_copy(void);
void test_arena_clear(void);
void test_arena_get_allocation_struct(void);


int passed_tests = 0;
int total_tests = 0;
int temp_passed;
int temp_total;


#define HELP_STRINGIFY(x) #x
#define STRINGIFY(x) HELP_STRINGIFY(x)


#define PRINT_BUFFER_SIZE 1024
char print_buffer[PRINT_BUFFER_SIZE];
int print_buffer_index = 0;


#define REPORT(msg)       print_buffer_index += snprintf(print_buffer + print_buffer_index, PRINT_BUFFER_SIZE - print_buffer_index, "  FAILURE: '%s' at %s:%d\n",       msg, __FILE__, __LINE__)
#define REPORT_FATAL(msg) print_buffer_index += snprintf(print_buffer + print_buffer_index, PRINT_BUFFER_SIZE - print_buffer_index, "  FATAL FAILURE: '%s' at %s:%d\n", msg, __FILE__, __LINE__)


#define TEST(exp, msg)                            \
    do                                            \
    {                                             \
        if(!(exp))                                \
        {                                         \
            REPORT(msg);                          \
        }                                         \
        else                                      \
        {                                         \
            passed_tests++;                       \
        }                                         \
        total_tests++;                            \
    }while(0)


#define TEST_FATAL(exp, msg) do{if(!(exp)){REPORT_FATAL(msg);return;}}while(0)


#define TEST_NULL(a)                TEST(a == NULL,         #a " is not NULL")
#define TEST_NOT_NULL(a)            TEST(a != NULL,         #a " is NULL")
#define TEST_EQUAL(a, b)            TEST(a == b,            #a " does not equal " #b)


#define TEST_ARRAY_EQUAL(a, b, s)                                           \
    do                                                                      \
    {                                                                       \
        int i;                                                              \
        total_tests++;                                                      \
        for(i = 0; i < s; i++)                                              \
        {                                                                   \
            if(a[i] != b[i])                                                \
            {                                                               \
                REPORT(#a " does not equal " #b);                           \
                break;                                                      \
            }                                                               \
        }                                                                   \
        passed_tests++;                                                     \
    }while(0)


#define SUITE(suite, name)                                                                                       \
    do                                                                                                           \
    {                                                                                                            \
        temp_passed = passed_tests;                                                                              \
        temp_total = total_tests;                                                                                \
        suite();                                                                                                 \
        fprintf(stderr, "Passed %d/%d tests in '%s'\n", passed_tests-temp_passed, total_tests-temp_total, name); \
        fprintf(stderr, "%s", print_buffer);                                                                     \
        print_buffer_index = 0;                                                                                  \
        print_buffer[0] = '\0';                                                                                  \
    } while(0)


#define ARENA_DEBUG
#define ARENA_IMPLEMENTATION
#define ARENA_SUPPRESS_MALLOC_WARN
#define ARENA_DEFAULT_ALIGNMENT 0
#include "arena.h"


#endif