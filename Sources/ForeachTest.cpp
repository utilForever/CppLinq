#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(Foreach, ThreeCharsSum)
{
	std::vector<char> src = { 'a', 'b', 'c' };

	std::string str = "";
	CppLinq::From(src).Foreach([&](char a) { str += a; });

	EXPECT_EQ("abc", str);
}