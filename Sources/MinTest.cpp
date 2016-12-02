#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(Min, ThreeInts)
{
	std::vector<int> src = { 1, 2, 3 };

	auto rng = CppLinq::From(src);

	EXPECT_EQ(1, rng.Min());
	EXPECT_EQ(3, rng.Min([](int a) { return -a; }));
}