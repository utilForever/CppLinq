#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(Sum, ThreeInts)
{
	std::vector<int> src = { 1, 2, 3 };

	auto rng = CppLinq::From(src).Sum();

	EXPECT_EQ(6, rng);
}