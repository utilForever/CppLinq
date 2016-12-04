#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(ToVector, Vector2Vector)
{
	std::vector<int> src = { 100, 200, 300 };

	auto rng = CppLinq::From(src);
	auto dst = rng.ToVector();

	EXPECT_EQ(dst, src);
}