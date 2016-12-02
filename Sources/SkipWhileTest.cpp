#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(SkipWhile, ManyToMore)
{
	int src[] = { 1, 2, 3, 4, 5, 6 };
	int ans[] = { 1, 2, 3, 4, 5, 6 };

	auto rng = CppLinq::From(src);
	auto dst = rng.SkipWhile([](int it) { return it < 0 || it > 10; });

	IsEqualArray(dst, ans);
}