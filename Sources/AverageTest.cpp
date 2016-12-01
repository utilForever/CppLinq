#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(Average, ThreeInts)
{
	std::vector<int> src = { 1, 2, 3 };

	auto rng = CppLinq::From(src);

	EXPECT_NEAR(2.0, rng.Average<double>(), DBL_EPSILON);
}