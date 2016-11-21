#ifndef CPP_LINQ_H
#define CPP_LINQ_H

#include <set>
#include <list>
#include <deque>
#include <vector>
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

		// Aggregate
		template <typename Ret>
		Ret Aggregate(Ret start, std::function<Ret(Ret, Type)> accumulate) const
		{
			try
			{
				auto en = m_enumerator;
				
				while (true)
				{
					start = accumulate(start, en.NextObject());
				}
			}
			catch (EnumeratorEndException&)
			{
				
			}

			return start;
		}

		// Elect
		Type Elect(std::function<Type(Type, Type)> accumulate) const
		{
			auto en = m_enumerator;
			Type result = en.NextObject();

			try
			{
				while (true)
				{
					result = accumulate(result, en.NextObject());
				}
			}
			catch (EnumeratorEndException&)
			{

			}

			return result;
		}

		// Export to container
		template <typename Container, typename Func>
		Container ExportToContainer(Func func) const
		{
			Container container;

			try
			{
				auto en = m_enumerator;

				while (true)
				{
					func(container, en.NextObject());
				}
			}
			catch (EnumeratorEndException&)
			{
				
			}

			return container;
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
		
		// Reverse
		LinqObject<Enumerator<Type, IteratorContainerPair<typename std::vector<Type>::const_reverse_iterator, std::vector<Type>>>> Reverse() const
		{
			using DataType = IteratorContainerPair<typename std::vector<Type>::const_reverse_iterator, std::vector<Type>>;

			return Enumerator<Type, DataType>([](DataType& pair)
			{
				return (pair.m_first == pair.m_second.crend()) ?
					throw EnumeratorEndException() : *(pair.m_first++);
			}, DataType(ToVector(), [](const std::vector<Type>& vec) {return vec.crbegin(); }));
		}

		// Sum
		template <typename Ret>
		Ret Sum(std::function<Ret(Type)> transform) const
		{
			return Aggregate<Ret>(Ret(), [=](Ret accumulator, Type object)
			{
				return accumulator + transform(object);
			});
		}

		template <typename Func>
		decltype(GetReturnType<Func, Type>()) Sum(Func transform) const
		{
			return Sum<decltype(GetReturnType<Func, Type>())>(transform);
		}

		template <typename Ret>
		Ret Sum() const
		{
			return Sum<Ret>([](Type a) { return a; });
		}

		Type Sum() const
		{
			return Sum<Type>();
		}

		// Average
		template <typename Ret>
		Ret Average(std::function<Ret(Type)> transform) const
		{
			int count = 0;

			return Aggregate<Ret>(Ret(), [&](Ret accumulator, Type object) -> Ret
			{
				count++;
				return (accumulator * (count - 1) + transform(object)) / count;
			});
		}

		template <typename Func>
		decltype(GetReturnType<Func, Type>()) Average(Func transform) const
		{
			return Average<decltype(GetReturnType<Func, Type>())>(transform);
		}

		template <typename Ret>
		Ret Average() const
		{
			return Average<Ret>([](Type a) { return a; });
		}

		Type Average() const
		{
			return Average<Type>();
		}

		// Count
		int Count(std::function<bool(Type)> predicate) const
		{
			return Aggregate<int>(0, [=](int count, Type a)
			{
				return count + (predicate(a) ? 1 : 0);
			});
		}

		int Count(const Type& value) const
		{
			return Count([=](Type a) { return a == value; });
		}

		int Count() const
		{
			return Count([]() { return true; });
		}

		// Any
		bool Any(std::function<bool(Type)> predicate) const
		{
			try
			{
				auto en = m_enumerator;

				while (true)
				{
					if (predicate(en.NextObject()))
					{
						return true;
					}
				}
			}
			catch (EnumeratorEndException&)
			{
				
			}

			return false;
		}

		bool Any() const
		{
			return Any([](Type a) { return static_cast<bool>(a); });
		}

		// All
		bool All(std::function<bool(Type)> predicate) const
		{
			return !Any([=](Type a) { return !predicate(a); });
		}

		bool All() const
		{
			return All([](Type a) { return static_cast<bool>(a); });
		}

		// Contains
		bool Contains(const Type& value) const
		{
			return Any([&](Type a) { return value == a; });
		}

		// Max
		template <typename Ret>
		Type Max(std::function<Ret(Type)> transform) const
		{
			return Elect([=](Type a, Type b) { return transform(a) < transform(b) ? b : a; });
		}

		template <typename Func>
		Type Max(Func transform) const
		{
			return Max<decltype(GetReturnType<Func, Type>())>(transform);
		}

		Type Max() const
		{
			return Max<Type>([](Type a) { return a; });
		}

		// Min
		template <typename Ret>
		Type Min(std::function<Ret(Type)> transform) const
		{
			return Elect([=](Type a, Type b) { return transform(a) < transform(b) ? a : b; });
		}

		template <typename Func>
		Type Min(Func transform) const
		{
			return Min<decltype(GetReturnType<Func, Type>())>(transform);
		}

		Type Min() const
		{
			return Min<Type>([](Type a) { return a; });
		}

		// ElementAt
		Type ElementAt(size_t index) const
		{
			auto en = m_enumerator;

			for (size_t i = 0; i < index; ++i)
			{
				en.NextObject();
			}

			return en.NextObject();
		}

		// First
		Type First(std::function<bool(Type)> predicate) const
		{
			return Where(predicate).m_enumerator.NextObject();
		}

		Type First() const
		{
			return First([]() { return true; });
		}

		Type FirstOrDefault(std::function<bool(Type)> predicate)
		{
			try
			{
				return First(predicate);
			}
			catch (EnumeratorEndException&)
			{
				return Type();
			}
		}

		Type FirstOrDefault() const
		{
			try
			{
				return First();
			}
			catch(EnumeratorEndException&)
			{
				return Type();
			}
		}

		// Last
		Type Last(std::function<bool(Type)> predicate) const
		{
			auto clause = Where(predicate);
			Type object = clause.m_enumerator.NextObject();

			try
			{
				while (true)
				{
					object = clause.m_enumerator.NextObject();
				}
			}
			catch (EnumeratorEndException&)
			{
				return object;
			}
		}

		Type Last() const
		{
			return Last([]() { return true; });
		}

		Type LastOrDefault(std::function<bool(Type)> predicate) const
		{
			try
			{
				return Last(predicate);
			}
			catch (EnumeratorEndException&)
			{
				return Type();
			}
		}

		Type LastOrDefault() const
		{
			return LastOrDefault([]() { return true; });
		}

		// Concat
		template <typename Enum2>
		LinqObject<Enumerator<Type, std::pair<bool, std::pair<Enum, Enum2>>>> Concat(LinqObject<Enum2> rhs) const
		{
			using DataType = std::pair<bool, std::pair<Enum, Enum2>>;

			return Enumerator<Type, DataType>([=](DataType& pair) -> Type
			{
				if (pair.m_first)
				{
					return pair.m_second.m_first.NextObject();
				}

				try
				{
					return pair.m_second.m_first.NextObject();
				}
				catch (EnumeratorEndException&)
				{
					pair.m_first = true;
					return pair.m_second.m_second.NextObject();
				}
			}, std::make_pair(false, std::make_pair(m_enumerator, rhs.m_enumerator)));
		}

		// Export methods
		std::vector<Type> ToVector() const
		{
			return ExportToContainer<std::vector<Type>>([](std::vector<Type>& container, const Type& value)
			{
				container.emplace_back(value);
			});
		}

		std::list<Type> ToList() const
		{
			return ExportToContainer<std::list<Type>>([](std::list<Type>& container, const Type& value)
			{
				container.emplace_back(value);
			});
		}

		std::deque<Type> ToDeque() const
		{
			return ExportToContainer<std::deque<Type>>([](std::deque<Type>& container, const Type& value)
			{
				container.emplace_back(value);
			});
		}

		std::set<Type> ToSet() const
		{
			return ExportToContainer<std::set<Type>>([](std::set<Type>& container, const Type& value)
			{
				container.insert(value);
			});
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

	// Repeat
	template <typename Type>
	LinqObject<Enumerator<Type, int>> Repeat(Type value, int count)
	{
		return Enumerator<Type, int>([=](int& index)
		{
			return (index++ >= count) ? throw EnumeratorEndException() : value;
		}, 0);
	}

	// Range
	template <typename Type>
	LinqObject<Enumerator<Type, std::pair<bool, Type>>> Range(Type begin, Type end, Type step)
	{
		return Enumerator<Type, std::pair<bool, Type>>([=](std::pair<bool, Type>& pair)
		{
			if (!(pair.m_second < end))
			{
				throw EnumeratorEndException();
			}

			if (!pair.m_first)
			{
				pair.m_frist = true;
			}
			else
			{
				pair.m_second += step;
			}

			return pair.m_second;
		}, std::make_pair(false, begin));
	}
}

#endif