#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(Reverse, IntVector)
{
	int src[] = { 1, 2, 3, 4 };
	int ans[] = { 4, 3, 2, 1 };

	auto rng = CppLinq::From(src);
	auto dst = rng.Reverse();

	IsEqualArray(dst, ans);
}