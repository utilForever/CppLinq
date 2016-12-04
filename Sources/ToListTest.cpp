#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(ToList, List2List)
{
	std::list<int> src = { 100, 200, 300 };

	auto rng = CppLinq::From(src);
	auto dst = rng.ToList();

	EXPECT_EQ(dst, src);
}