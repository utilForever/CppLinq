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
		using Type = typename Enum::value_type;

		template <typename Func, typename Arg>
		static auto GetReturnType(Func* func = nullptr, Arg* arg1 = nullptr)
			-> decltype((*func)(*arg1));

		template <typename Func, typename Arg1, typename Arg2>
		static auto GetReturnType(Func* func = nullptr, Arg1* arg1 = nullptr, Arg2* arg2 = nullptr)
			-> decltype((*func)(*arg1, *arg2));

		// Select Internal
		template <typename Ret>
		LinqObject<Enumerator<Ret, std::pair<Enum, int>>> SelectInternal(std::function<Ret(Type, int)> transform) const
		{
			return Enumerator<Ret, std::pair<Enum, int>>([=](std::pair<Enum, int>& pair) -> Ret
			{
				return transform(pair.first.NextObject(), pair.second++);
			}, std::make_pair(m_enumerator, 0));
		}

		template <typename Func>
		LinqObject<Enumerator<decltype(GetReturnType<Func, Type, int>()), std::pair<Enum, int>>> SelectInternal(Func transform) const
		{
			return SelectInternal<decltype(GetReturnType<Func, Type, int>())>(transform);
		}

	public:
		Enum m_enumerator;

		// Select
		template <typename Ret>
		LinqObject<Enumerator<Ret, std::pair<Enum, int>>> Select(std::function<Ret(Type)> transform) const
		{
			return SelectInternal<Ret>([=](Type a, int) { return transform(a); });
		}

		template <typename Func>
		LinqObject<Enumerator<decltype(GetReturnType<Func, Type>()), std::pair<Enum, int>>> Select(Func transform) const
		{
			return Select<decltype(GetReturnType<Func, Type>())>(transform);
		}
	};

	// From
	template <typename Type, typename Iter>
	LinqObject<Enumerator<Type, Iter>> From(Iter begin, Iter end)
	{
		return Enumerator<Type, Iter>([=](Iter& iter)
		{
			return (iter == end) ? throw EnumeratorEndException() : *(iter++);
		}, begin);
	}

	template <typename Type, typename Iter>
	LinqObject<Enumerator<Type, std::pair<Iter, int>>> From(Iter begin, int length)
	{
		return Enumerator<Type, std::pair<Iter, int>>([=](std::pair<Iter, int>& pair)
		{
			return (pair.second++ == length) ? throw EnumeratorEndException() : *(pair.first++);
		}, std::make_pair(begin, 0));
	}

	template <typename Type, int N>
	auto From(Type (&array)[N])
		-> decltype(From<Type>(array, array + N))
	{
		return From<Type>(array, array + N);
	}

	template<template<class> class Container, class Type>
	auto From(const Container<Type>& container)
		-> decltype(From<Type>(std::begin(container), std::end(container)))
	{
		return From<Type>(std::begin(container), std::end(container));
	}
}

#endif