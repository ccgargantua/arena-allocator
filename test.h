#ifndef TEST_H
#define TEST_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static int passed_tests = 0;
static int total_tests = 0;
static int temp_passed;
static int temp_total;


#define REPORT(msg) fprintf(stderr, "FAILURE: '%s' at %s:%d\n", msg, __FILE__, __LINE__)
#define REPORT_FATAL(msg) fprintf(stderr, "FATAL FAILURE: '%s' at %s:%d\n", msg, __FILE__, __LINE__)


#define TEST(exp, msg)      \
    do                      \
    {                       \
        if(!(exp))          \
        {                   \
            REPORT(msg);    \
        }                   \
        else                \
        {                   \
            passed_tests++; \
        }                   \
        total_tests++;      \
    }while(0)


#define TEST_FATAL(exp, msg) do{if(!(exp)){REPORT_FATAL(msg);abort();}}while(0)


#define TEST_NULL(a)            TEST(a == NULL, #a " is not NULL")
#define TEST_NOT_NULL(a)        TEST(a != NULL, #a " is NULL")
#define TEST_EQUAL(a, b)        TEST(a == b, #a " does not equal " #b)
#define TEST_NOT_EQUAL(a, b)    TEST(a != b, #a " equals " #b)
#define TEST_STRING_EQUAL(a, b) TEST(strcmp(a, b) == 0, #a " does not equal " #b)
#define TEST_STRING_NOT_EQUAL   TEST(strcmp(a, b) != 0, #a " equals " #b)


#define TEST_ARRAY_EQUAL(a, b, s)                                 \
    do                                                            \
    {                                                             \
        int i;                                                    \
        int current_difference = total_tests - passed_tests;      \
        for(i = 0; i < s; i++)                                    \
        {                                                         \
            TEST_EQUAL(a[i], b[i]);                               \
            if(total_tests - passed_tests != current_difference)  \
            {                                                     \
                REPORT(#a " does not equal " #b);                 \
                break;                                            \
            }                                                     \
        }                                                         \
    }while(0)


#define SUITE(suite, name)                                                                                       \
    do                                                                                                           \
    {                                                                                                            \
        temp_passed = passed_tests;                                                                              \
        temp_total = total_tests;                                                                                \
        suite();                                                                                                 \
        fprintf(stderr, "Passed %d/%d tests in '%s'\n", passed_tests-temp_passed, total_tests-temp_total, name); \
    } while(0)


#define ARENA_DEBUG
#define ARENA_IMPLEMENTATION
#define ARENA_SUPPRESS_MALLOC_WARN
#include "arena.h"


#endif