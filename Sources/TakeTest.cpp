#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(Take, ManyToMore)
{
	int src[] = { 1, 2, 3, 4, 5, 6 };
	int ans[] = { 1, 2, 3, 4, 5, 6 };

	auto rng = CppLinq::From(src);
	auto dst = rng.Take(10);

	IsEqualArray(dst, ans);
}