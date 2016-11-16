#ifndef CPP_LINQ_TEST_UTILS_H
#define CPP_LINQ_TEST_UTILS_H

#include "CppLinq.h"

template<typename R, typename T, unsigned N, typename F>
void IsEqualArray(R dst, T(&ans)[N], F f)
{
	for (unsigned i = 0; i < N; i++)
		EXPECT_EQ(f(ans[i]), f(dst.NextObject()));

	EXPECT_THROW(dst.NextObject(), CppLinq::EnumeratorEndException);
}

template<typename R, typename T, unsigned N>
void IsEqualArray(R dst, T(&ans)[N])
{
	for (unsigned i = 0; i < N; i++)
		EXPECT_EQ(ans[i], dst.NextObject());

	EXPECT_THROW(dst.NextObject(), CppLinq::EnumeratorEndException);
}

#endif