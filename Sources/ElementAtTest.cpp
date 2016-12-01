#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(ElementAt, ThreeInts)
{
	std::vector<int> src = { 1, 2, 3 };

	auto rng = CppLinq::From(src);

	EXPECT_EQ(1, rng.ElementAt(0));
	EXPECT_EQ(2, rng.ElementAt(1));
	EXPECT_EQ(3, rng.ElementAt(2));
}