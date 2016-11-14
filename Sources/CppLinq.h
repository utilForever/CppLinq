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
}

#endif