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

	std::vector<People> peoples = { {1, "Tom"}, {2, "Mike"}, {3, "Susan"} };
	std::vector<Account> accounts = { {2, 10000}, {3, 20000}, {4, 30000} };

	auto dst = CppLinq::LeftJoin(peoples, accounts, [](People p, Account a) { p.m_no = a.m_no; });
}