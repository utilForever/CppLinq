#include <gtest/gtest.h>

#include "CppLinq.h"
#include "TestUtils.h"

TEST(LeftJoin, People)
{
	struct People
	{
		int m_no;
		std::string m_name;
	};

	struct Account
	{
		int m_no;
		int m_money;
	};

	struct PeopleAccount
	{
		int m_no;
		std::string m_name;
		int m_money;
	};

	People peoples[] = { {1, "Tom"}, {2, "Mike"}, {3, "Susan"} };
	Account accounts[] = { {2, 10000}, {3, 20000}, {4, 30000} };
	PeopleAccount ans[] = { { 1, "Tom", NULL }, { 2, "Mike", 10000 }, { 3, "Susan", 20000 } };

	auto rng1 = CppLinq::From(peoples);
	auto rng2 = CppLinq::From(accounts);

	auto dst = rng1.LeftJoin(rng2, [](People p, Account a) { return p.m_no == a.m_no; });

	IsEqualArray(dst, ans);
}