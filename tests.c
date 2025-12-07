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
	char region[256];
	Arena arena;
	arena_init(&arena, region, 256);
	char *bytes = arena_alloc(&arena, 8);
	ASSERT_TRUE(bytes != NULL);
	EXPECT_TRUE(arena.region == bytes);
	EXPECT_LONG_EQ(arena.index, 8);
	EXPECT_LONG_EQ(arena.allocations, 1);
}
