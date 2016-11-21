#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(All, ThreeInts)
{
	std::vector<int> src = { 1, 2, 3 };

	auto rng = CppLinq::From(src);

	EXPECT_TRUE(rng.All());

	EXPECT_TRUE(rng.All([](int a) { return a > 0; }));
	EXPECT_TRUE(rng.All([](int a) { return a < 4; }));
	EXPECT_TRUE(rng.All([](int a) { return a > 0 && a < 4; }));

	EXPECT_FALSE(rng.All([](int a) { return a > 2; }));
	EXPECT_FALSE(rng.All([](int a) { return a == 1; }));
	EXPECT_FALSE(rng.All([](int a) {return a < 3; }));
}