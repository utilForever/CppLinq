#ifndef CPP_LINQ_H
#define CPP_LINQ_H

#include <iostream>
#include <functional>

namespace CppLinq
{
	// Enumerator End Exception
	class EnumeratorEndException { };

	// Enumerator Class Template
	template <typename Ret, typename Arg>
	class Enumerator
	{
	public:
		Enumerator(std::function<Ret(Arg&)> nextObject, Arg data) :
			m_nextObject(nextObject), m_data(data)
		{
			
		}

		Ret NextObject()
		{
			return m_nextObject(m_data);
		}

	private:
		std::function<Ret(Arg&)> m_nextObject;
		Arg m_data;
	};

	template <typename Ret, typename Arg>
	std::ostream& operator<<(std::ostream& stream, Enumerator<Ret, Arg> enumerator)
	{
		try
		{
			while (true)
			{
				stream << enumerator.NextObject() << ' ';
			}
		}
		catch (EnumeratorEndException& e)
		{
			
		}

		return stream;
	}
	
	// Linq Object
	template<typename Enum>
	class LinqObject
	{

	};

	// From
	template <typename Type, typename Iter>
	LinqObject<Enumerator<Type, Iter>> from(Iter begin, Iter end)
	{
		return Enumerator<Type, Iter>([=](Iter& iter)
		{
			return (iter == end) ? throw EnumeratorEndException() : *(iter++);
		}, begin);
	}

	template <typename Type, typename Iter>
	LinqObject<Enumerator<Type, std::pair<Iter, int>>> from(Iter begin, int length)
	{
		return Enumerator<Type, std::pair<Iter, int>>([=](std::pair<Iter, int>& pair)
		{
			return (pair.second++ == length) ? throw EnumeratorEndException() : *(pair.first++);
		}, std::make_pair(begin, 0));
	}
}

#endif