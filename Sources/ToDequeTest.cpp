#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(ToDeque, Deque2Deque)
{
	std::deque<int> src = { 100, 200, 300 };

	auto rng = CppLinq::From(src);
	auto dst = rng.ToDeque();

	EXPECT_EQ(dst, src);
}