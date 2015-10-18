#include<string>
#include<vector>
#include<list>
#include<iostream>
#include<type_traits>
#include<array>
#include<memory>
#include<cassert>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>

template <typename arg_itr, typename functor>
struct lazy_eval_iterator : boost::iterator_facade<
								lazy_eval_iterator<arg_itr, functor>,
								typename std::result_of<functor(arg_itr)>::type,
								boost::single_pass_traversal_tag,
								typename std::add_lvalue_reference<typename std::result_of<functor(arg_itr)>::type>::type,
								std::ptrdiff_t
							>
{
	using expr_type = functor;
	using this_type = lazy_eval_iterator<arg_itr, functor>;
	arg_itr arg;
	functor expr;
	using value_type = typename std::result_of<functor(arg_itr)>::type;

	//mutable so we can cache the value
	mutable value_type result;
	mutable bool has_been_evaluated;

	lazy_eval_iterator(arg_itr arg_, functor expr_) : arg(arg_), expr(expr_), has_been_evaluated(false)
	{
	}

	lazy_eval_iterator(const this_type& other) : arg(other.arg), expr(other.expr), result(other.result), has_been_evaluated(other.has_been_evaluated)
	{
	}

	//const - whatever
	value_type& operator*() const
	{
		if (!has_been_evaluated)
		{
			result = expr(arg);
			has_been_evaluated = true;
		}

		return result;
	}

	value_type& operator*()
	{
		if (!has_been_evaluated)
		{
			result = expr(arg);
			has_been_evaluated = true;
		}

		return result;
	}

	inline this_type& operator++()
	{
		++arg;
		has_been_evaluated = false;
		return *this;
	}

	inline this_type operator++(int)
	{
		auto result = this_type(arg, expr);
		++arg;
		has_been_evaluated = false;
		return result;
	}

	inline bool operator!=(this_type other) const
	{
		return arg != other.arg;
	}

	//const - whatever
	value_type& dereference() const
	{
		return **this;
	}

	value_type& dereference()
	{
		return **this;
	}

	bool equal(this_type other) const
	{
		return arg == other.arg;
	}

	auto& increment() const
	{
		return ++(*this);
	}
};

template <typename arg_itr, typename functor>
lazy_eval_iterator<arg_itr, functor> make_lazy_eval_iterator(arg_itr itr, functor f)
{
	return lazy_eval_iterator<arg_itr, functor>(itr, f);
}

template<typename itr>
struct basic_range
{
	using this_type = basic_range<itr>;
	using iterator = itr;

	itr _begin;
	itr _end;
	using value_type = decltype(*_begin);
	using optional_type = boost::optional<value_type>;

	auto begin()
	{
		return _begin;
	}

	auto end()
	{
		return _end;
	}
	
	auto& first()
	{
		return *_begin;
	}

	auto count() const
	{
		auto count = 0;
		auto temp = _begin;

		while (temp != _end)
		{
			++temp;
			++count;
		}

		return count;
	}

	template<typename functor>
	optional_type optional_first(functor f)
	{
		for (auto& x : *this)
		{
			if (f(x))
			{
				return x;
			}
		}

		return optional_type();
	}

	optional_type optional_first()
	{
		if (_begin != _end)
		{
			return first();
		}

		return optional_type();
	}

	auto& to_vector()
	{
		auto result = vector<typename std::remove_reference<value_type>::type>(count());
		std::copy(begin(), end(), std::back_inserter<decltype(result)>(result));
		return std::move(result);
	}

	template<typename select_funtor>
	auto select_(select_funtor f) -> decltype(make_basic_lazy_range(_begin, _end, f))
	{
		return make_basic_lazy_range(_begin, _end, f);
	}

	//todo
	//template<typename select_funtor>
	//auto select_many(select_funtor f) -> decltype(make_basic_lazy_range(_begin, _end, f))
	//{
	//	return make_basic_lazy_range(_begin, _end, f);
	//}

	template<typename where_functor>
	auto where_(where_functor f)
	{
		//todo: implement my own version of this so it can be lazy - coroutines?
		auto filtered_begin = boost::make_filter_iterator<decltype(f), itr>(f, _begin, _end);
		auto filtered_end = boost::make_filter_iterator<decltype(f), itr>(f, _end, _end);
		return basic_range<decltype(filtered_begin)>(filtered_begin, filtered_end);
	}

	template<typename functor>
	auto for_each_(functor f) const
	{
		std::for_each(_begin, _end, f);
		return *this;
	}

	template<typename functor>
	auto call_once_(functor f) const
	{
		f();
		return *this;
	}

	bool has_any()
	{
		return _begin != _end;
	}

	template<typename functor>
	bool none_(functor f)
	{
		return !any_<functor>(f);
	}

	template<typename functor>
	bool any_(functor f)
	{
		for (auto& x : *this)
		{
			if (f(x))
			{
				return true;
			}
		}

		return false;
	}

	template<typename other>
	bool contains(other item)
	{
		for (auto& x : *this)
		{
			if (item == x)
			{
				return true;
			}
		}

		return false;
	}

	bool contains(const value_type& item)
	{
		for (auto& x : *this)
		{
			if (item == x)
			{
				return true;
			}
		}

		return false;
	}

	basic_range(itr begin_, itr end_) : _begin(begin_), _end(end_)
	{
	}

	basic_range(const this_type& other) : _begin(other._begin), _end(other._end)
	{
	}

	basic_range()
	{
	}
};

template<typename itr, typename functor>
auto make_basic_lazy_range(itr begin, itr end, functor f)
{
	using lazy_itr = decltype(make_lazy_eval_iterator(begin, f));
	return basic_range<lazy_itr>(make_lazy_eval_iterator(begin, f), make_lazy_eval_iterator(end, f));
}

template<typename itr>
auto make_basic_range(itr begin, itr end)
{
	return basic_range<itr>(begin, end);
}

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::array;
using std::string;

//you can format these macros however you prefer.
#define select(itr, expr) select_([&](const auto& __itr){auto& itr = *__itr; return (expr);})
#define where(itr, expr) where_([&](const auto& itr){return (expr);})
#define from(container) make_basic_range(container.begin(), container.end())
#define for_each(itr, expr) for_each_([&](const auto& itr){(expr);})
#define call_once(expr) call_once_([&]{(expr);})
#define any(itr, expr) any_([&](const auto& itr){return (expr);})
#define none(itr, expr) none_([&](const auto& itr){return (expr);})
#define first_where(itr, expr) optional_first([&](const auto& itr){return (expr);})
#define first_or_default() optional_first()

struct foo
{
	const int number;
	int get_number() { return number; }
	foo(int d) :number(d) {}
};

string uppercase(string s)
{
	for (auto& c : s)
	{
		c = toupper(c);
	}

	return s;
}

int main()
{
	auto foos = vector<foo>{ foo(0), foo(1), foo(2), foo(3), foo(4) };
	auto strings = array<string, 5>{ "bob", "sally", "john", "cindy", "mark"};

	auto johnless = from(foos)
				    .for_each(f, cout << "foo numbers: " << f.number << endl)
				    .select(f, strings[f.number])
				    .for_each(s, cout << "strings: " << s << endl)
				    .where(s, s != "john");

	auto john_string =   from(strings)
						 .first_where(s, !johnless.contains(s));

	cout << "john string: " << john_string << endl;
				 
                 

	cout << endl << "count: " << johnless.count() << endl << endl;

	for (auto element : johnless)
	{
		cout << element << endl;
	}

	auto vec = johnless.to_vector();

	for (auto element : vec)
	{
		cout << element << endl;
	}

	cout << endl;

	//without macros
	auto ugly_range = make_basic_range(foos.begin(), foos.end())
                        .select_([&](const auto& x) {return strings[(*x).number]; })
                        .where_([&](const auto& s) {return s != "john"; })
                        .select_([&](const auto& s) {return uppercase(*s); })
                        .for_each_([&](const auto& s) {cout << s << endl; });

	cin.get();
	return 0;
}