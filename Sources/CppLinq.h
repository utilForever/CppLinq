#ifndef CPP_LINQ_H
#define CPP_LINQ_H

#include <set>
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
		using value_type = Ret;

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
		catch (EnumeratorEndException&)
		{
			
		}

		return stream;
	}

	// Iterator and Container Pair
	template <typename Iter, typename Container>
	class IteratorContainerPair
	{
		std::function<Iter(const Container&)> m_getIter;

	public:
		Container m_second;
		Iter m_first;

		IteratorContainerPair(const Container& container, std::function<Iter(const Container&)> getIter) :
			m_getIter(getIter), m_second(container), m_first(m_getIter(m_second))
		{
			
		}

		IteratorContainerPair(const IteratorContainerPair<Iter, Container>& pair) :
			m_getIter(pair.m_getIter), m_second(pair.m_second), m_first(m_getIter(m_second))
		{
			for (auto iter = pair.m_getIter(pair.m_second); iter != pair.m_first; ++iter)
			{
				++m_first;
			}
		}
	};
	
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

		// Where Internal
		LinqObject<Enumerator<Type, std::pair<Enum, int>>> WhereInternal(std::function<bool(Type, int)> predicate) const
		{
			return Enumerator<Type, std::pair<Enum, int>>([=](std::pair<Enum, int>& pair) -> Type
			{
				Type object;

				do
				{
					object = pair.first.NextObject();
				} while (!predicate(object, pair.second++));

				return object;
			}, std::make_pair(m_enumerator, 0));
		}

		// Foreach Internal
		void ForeachInternal(std::function<void(Type, int)> action) const
		{
			auto en = m_enumerator;
			int index = 0;

			try
			{
				while (true)
				{
					action(en.NextObject(), index++);
				}
			}
			catch (EnumeratorEndException&)
			{
				
			}
		}

		// TakeWhile Internal
		LinqObject<Enumerator<Type, std::pair<Enum, int>>> TakeWhileInternal(std::function<bool(Type, int)> predicate) const
		{
			return Enumerator<Type, std::pair<Enum, int>>([=](std::pair<Enum, int>& pair) -> Type
			{
				Type object = pair.m_first.NextObject();

				if (!predicate(object, pair.m_second++))
				{
					throw EnumeratorEndException();
				}

				return object;
			}, std::make_pair(m_enumerator, 0));
		}

		// SkipWhile Internal
		LinqObject<Enumerator<Type, std::pair<Enum, int>>> SkipWhileInternal(std::function<bool(Type, int)> predicate) const
		{
			return Enumerator<Type, std::pair<Enum, int>>([=](std::pair<Enum, int>& pair) -> Type
			{
				if (pair.m_second != 0)
				{
					return pair.m_first.NextObject();
				}

				Type object;

				do
				{
					object = pair.m_first.NextObject();
				} while (predicate(object, pair.m_second++));

				return object;
			}, std::make_pair(m_enumerator, 0));
		}

	protected:
		template <typename Type, typename Ret>
		class TransformComparer
		{
		public:
			std::function<Ret(Type)> m_func;

			TransformComparer(std::function<Ret(Type)> func) :
				m_func(func)
			{
				
			}

			bool operator()(const Type& a, const Type& b) const
			{
				return m_func(a) < m_func(b);
			}
		};

	public:
		Enum m_enumerator;

		using value_type = typename Enum::value_type;

		LinqObject(Enum enumerator) : m_enumerator(enumerator)
		{

		}

		Type NextObject()
		{
			return m_enumerator.NextObject();
		}

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

		// Where
		LinqObject<Enumerator<Type, std::pair<Enum, int>>> Where(std::function<bool(Type)> predicate) const
		{
			return WhereInternal([=](Type a, int) { return predicate(a); });
		}

		// OrderBy
		template <typename Ret>
		LinqObject<Enumerator<Type, IteratorContainerPair<typename std::multiset<Type, TransformComparer<Type, Ret>>::iterator,
			std::multiset<Type, TransformComparer<Type, Ret>>>>> OrderBy(std::function<Ret(Type)> transform) const
		{
			using DataType = IteratorContainerPair<typename std::multiset<Type, TransformComparer<Type, Ret>>::iterator,
				std::multiset<Type, TransformComparer<Type, Ret>>>;

			std::multiset<Type, TransformComparer<Type, Ret>> objects(transform);

			try
			{
				auto en = m_enumerator;
				while (true)
				{
					objects.insert(en.NextObject());
				}
			}
			catch (EnumeratorEndException&)
			{
				
			}

			return Enumerator<Type, DataType>([](DataType& pair)
			{
				return (pair.m_first == pair.m_second.end()) ?
					throw EnumeratorEndException() : *(pair.m_first++);
			}, DataType(objects, [](const std::multiset<Type, TransformComparer<Type, Ret>>& mulSet) { return mulSet.begin(); }));
		}

		template <typename Func>
		LinqObject<Enumerator<Type, IteratorContainerPair<typename std::multiset<Type, TransformComparer<Type, decltype(GetReturnType<Func, Type>())>>::iterator,
			std::multiset<Type, TransformComparer<Type, decltype(GetReturnType<Func, Type>())>>>>> OrderBy(Func transform) const
		{
			return OrderBy<decltype(GetReturnType<Func, Type>())>(transform);
		}

		LinqObject<Enumerator<Type, IteratorContainerPair<typename std::multiset<Type, TransformComparer<Type, Type>>::iterator,
			std::multiset<Type, TransformComparer<Type, Type>>>>> OrderBy() const
		{
			return OrderBy<Type>([](Type a) { return a; });
		}

		// Foreach
		void Foreach(std::function<void(Type)> action) const
		{
			ForeachInternal([=](Type a, int) { return action(a); });
		}

		// Take
		LinqObject<Enumerator<Type, std::pair<Enum, int>>> Take(int count) const
		{
			return WhereInternal([=](Type, int i)
			{
				if (i == count)
				{
					throw EnumeratorEndException();
				}

				return true;
			});
		}

		// TakeWhile
		LinqObject<Enumerator<Type, std::pair<Enum, int>>> TakeWhile(std::function<bool(Type)> predicate) const
		{
			return TakeWhileInternal([=](Type t, int) { return predicate(t); });
		}

		// Skip
		LinqObject<Enumerator<Type, std::pair<Enum, int>>> Skip(int count) const
		{
			return WhereInternal([=](Type, int i) { return i >= count; });
		}

		// SkipWhile
		LinqObject<Enumerator<Type, std::pair<Enum, int>>> SkipWhile(std::function<bool(Type)> predicate) const
		{
			return SkipWhileInternal([=](Type t) { return predicate(t); });
		}

		// Cast
		template <typename Ret>
		LinqObject<Enumerator<Ret, std::pair<Enum, int>>> Cast() const
		{
			return SelectInternal<Ret>([=](Type a) { return a; });
		}

		// Distinct
		template <typename Ret>
		LinqObject<Enumerator<Type, std::pair<Enum, std::set<Ret>>>> Distinct(std::function<Ret(Type)> transform) const
		{
			using DataType = std::pair<Enum, std::set<Ret>>;

			return Enumerator<Type, DataType>([=](DataType& pair) -> Type
			{
				while (true)
				{
					Type object = pair.m_first.NextObject();
					Ret key = transform(object);

					if (pair.m_second.find(key) == pair.m_second.end())
					{
						pair.m_second.insert(key);
						return object;
					}
				}
			}, std::make_pair(m_enumerator, std::set<Ret>()));
		}

		template <typename Func>
		LinqObject<Enumerator<Type, std::pair<Enum, std::set<decltype(GetReturnType<Func, Type>())>>>> Distinct(Func transform) const
		{
			return Distinct<decltype(GetReturnType<Func, Type>())>(transform);
		}

		LinqObject<Enumerator<Type, std::pair<Enum, std::set<Type>>>> Distinct() const
		{
			return Distinct<Type>([](Type a) { return a; });
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

	template <template <class> class Container, class Type>
	auto From(const Container<Type>& container)
		-> decltype(From<Type>(std::begin(container), std::end(container)))
	{
		return From<Type>(std::begin(container), std::end(container));
	}

	// For std::list, std::vector, std::dequeue
	template<template<class, class> class V, typename T, typename U>
	auto From(const V<T, U>& container)
		-> decltype(From<T>(std::begin(container), std::end(container)))
	{
		return From<T>(std::begin(container), std::end(container));
	}

	// For std::set
	template <template <class, class, class> class V, typename T, typename S, typename U>
	auto From(const V<T, S, U>& container)
		-> decltype(From<T>(std::begin(container), std::end(container)))
	{
		return From<T>(std::begin(container), std::end(container));
	}

	// For std::map
	template <template <class, class, class, class> class V, typename K, typename T, typename S, typename U>
	auto From(const V<K, T, S, U>& container)
		-> decltype(From<std::pair<K, T>>(std::begin(container), std::end(container)))
	{
		return From<std::pair<K, T>>(std::begin(container), std::end(container));
	}

	// For std::array
	template <template <class, size_t> class V, typename T, size_t L>
	auto From(const V<T, L>& container)
		-> decltype(From<T>(std::begin(container), std::end(container)))
	{
		return From<T>(std::begin(container), std::end(container));
	}
}

#endif