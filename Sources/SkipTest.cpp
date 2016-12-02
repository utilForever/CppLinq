#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(Skip, ManyToMany)
{
	int src[] = { 1, 2, 3, 4, 5, 6 };
	int ans[] = { 1, 2, 3, 4, 5, 6 };

	auto rng = CppLinq::From(src);
	auto dst = rng.Skip(0);

	IsEqualArray(dst, ans);
}