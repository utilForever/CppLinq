#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(OrderBy, RandomIntsWithDuplicates)
{
	int src[] = { 4, 5, 3, 1, 4, 2, 1, 4, 6 };
	int ans[] = { 1, 1, 2, 3, 4, 4, 4, 5, 6 };

	auto rng = CppLinq::From(src);
	auto dst = rng.OrderBy();

	IsEqualArray(dst, ans);
}