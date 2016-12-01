#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(Concat, ArrayArray)
{
	int src1[] = { 1, 2, 3, 4, 5 };
	int src2[] = { 6, 7, 8, 9 };

	int ans[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

	auto rng1 = CppLinq::From(src1);
	auto rng2 = CppLinq::From(src2);
	auto dst = rng1.Concat(rng2);

	IsEqualArray(dst, ans);
}