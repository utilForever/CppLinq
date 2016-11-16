#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(Select, Multiply2)
{
	int src[] = { 1, 2, 3, 4 };
	int ans[] = { 2, 4, 6, 8 };

	auto rng = CppLinq::From(src);
	auto dst = rng.Select([](int a) { return a * 2; });

	IsEqualArray(dst, ans);
}