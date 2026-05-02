#include "rktest.h"


#define ARENA_DEBUG
#include "arena.h"


TEST(arena_init_tests, init)
{
	Arena arena;
	char data[256];
	arena_init(&arena, data, 256);
	EXPECT_TRUE(arena.region == data);
	EXPECT_LONG_EQ(arena.size, 256);
	EXPECT_LONG_EQ(arena.index, 0);
}


TEST(arena_create_tests, basic)
{
	Arena *arena = arena_create(8);
	ASSERT_TRUE(arena != NULL);
	EXPECT_LONG_EQ(arena->size, 8);
	arena_destroy(arena);
}


TEST(arena_create_tests, zero_bytes)
{
	ASSERT_TRUE(arena_create(0) == NULL);
}


TEST(arena_alloc_tests, basic)
{
	Arena arena;
	char data[64];
	arena_init(&arena, data, 64);
	char *p = arena_alloc(&arena, 8);
	ASSERT_TRUE(p != NULL);
	EXPECT_LONG_EQ(arena.index, 8);
}


TEST(arena_alloc_tests, full)
{
	Arena arena;
	char data[16];
	arena_init(&arena, data, 16);
	ASSERT_TRUE(arena_alloc(&arena, 16) != NULL);
	ASSERT_TRUE(arena_alloc(&arena, 1) == NULL);
	EXPECT_LONG_EQ(arena.index, 16);
}


TEST(arena_alloc_tests, zero)
{
	Arena arena;
	char data[16];
	arena_init(&arena, data, 16);
	ASSERT_TRUE(arena_alloc(&arena, 0) != NULL);
}


TEST(arena_alloc_tests, overflow)
{
	Arena arena;
	char data[16];
	arena_init(&arena, data, 16);
	char *p = arena_alloc(&arena, SIZE_MAX);
	ASSERT_TRUE(p == NULL);
}


TEST(arena_alloc_aligned_tests, basic)
{
	Arena arena;
	char data[64];
	arena_init(&arena, data, 64);
	char *p = arena_alloc_aligned(&arena, 16, 8);
	ASSERT_TRUE(p != NULL);
	EXPECT_LONG_EQ((uintptr_t)p % 8, 0);
	EXPECT_LONG_EQ(arena.index, 16);
}


TEST(arena_alloc_aligned_tests, align_1)
{
	Arena arena;
	char data[32];
	arena_init(&arena, data, 32);
	char *p = arena_alloc_aligned(&arena, 8, 1);
	ASSERT_TRUE(p != NULL);
	EXPECT_LONG_EQ(arena.index, 8);
}


TEST(arena_alloc_aligned_tests, multiple)
{
	Arena arena;
	char data[128];
	arena_init(&arena, data, 128);
	char *a = arena_alloc_aligned(&arena, 8, 16);
	char *b = arena_alloc_aligned(&arena, 8, 16);
	ASSERT_TRUE(a != NULL);
	ASSERT_TRUE(b != NULL);
	EXPECT_LONG_EQ((uintptr_t)a % 16, 0);
	EXPECT_LONG_EQ((uintptr_t)b % 16, 0);
	EXPECT_TRUE(b > a);
}


TEST(arena_copy_tests, basic)
{
	Arena arena;
	char data[64];
	arena_init(&arena, data, 64);
	const char *src = "hello";
	char *dst = arena_copy(&arena, src, 6);
	ASSERT_TRUE(dst != NULL);
	EXPECT_STR_EQ(dst, "hello");
	EXPECT_LONG_EQ(arena.index, 6);
}


TEST(arena_copy_tests, null_src)
{
	Arena arena;
	char data[64];
	arena_init(&arena, data, 64);
	char *dst = arena_copy(&arena, NULL, 4);
	ASSERT_TRUE(dst == NULL);
}


TEST(arena_copy_tests, zero_bytes)
{
	Arena arena;
	char data[64];
	arena_init(&arena, data, 64);
	char buf[] = "test";
	char *dst = arena_copy(&arena, buf, 0);
	ASSERT_TRUE(dst == NULL);
}


TEST(arena_copy_tests, into_full_arena)
{
	Arena arena;
	char data[8];
	arena_init(&arena, data, 8);
	arena_alloc(&arena, 8);
	char *dst = arena_copy(&arena, "abc", 4);
	ASSERT_TRUE(dst == NULL);
}


TEST(arena_clear_tests, reset_index)
{
	Arena arena;
	char data[64];
	arena_init(&arena, data, 64);
	arena_alloc(&arena, 32);
	EXPECT_LONG_EQ(arena.index, 32);
	arena_clear(&arena);
	EXPECT_LONG_EQ(arena.index, 0);
}


TEST(arena_clear_tests, reuse_after_clear)
{
	Arena arena;
	char data[64];
	arena_init(&arena, data, 64);
	char *a = arena_alloc(&arena, 16);
	ASSERT_TRUE(a != NULL);
	arena_clear(&arena);
	char *b = arena_alloc(&arena, 16);
	ASSERT_TRUE(b != NULL);
	EXPECT_TRUE(a == b);
}


TEST(arena_destroy_tests, null_arena)
{
	arena_destroy(NULL);
}


TEST(arena_destroy_tests, dynamic)
{
	Arena *arena = arena_create(32);
	ASSERT_TRUE(arena != NULL);
	arena_destroy(arena);
}


TEST(edge_case_tests, large_alloc)
{
	Arena arena;
	char data[1024];
	arena_init(&arena, data, 1024);
	char *p = arena_alloc(&arena, 1023);
	ASSERT_TRUE(p != NULL);
	EXPECT_LONG_EQ(arena.index, 1023);
}
