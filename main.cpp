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

template <typename arg_itr, typename functor>
struct lazy_filter_iterator : boost::iterator_facade<
	lazy_filter_iterator<arg_itr, functor>,
	typename std::result_of<functor(arg_itr)>::type,
	boost::single_pass_traversal_tag,
	typename std::add_lvalue_reference<typename std::result_of<functor(arg_itr)>::type>::type,
	std::ptrdiff_t
>
{
	using expr_type = functor;
	using this_type = lazy_filter_iterator<arg_itr, functor>;

	mutable arg_itr arg; // :(
	functor expr;
	arg_itr end;
	using value_type = decltype(*arg);

	lazy_filter_iterator(arg_itr arg_, arg_itr end_, functor expr_) : arg(arg_), end(end_), expr(expr_)
	{
		increment_to_next_match();
	}

	lazy_filter_iterator(const this_type& other) : arg(other.arg), end(other.end), expr(other.expr)
	{
	}

	void increment_to_next_match() const
	{
		while (arg != end && !expr(arg))
		{
			++arg;
		}
	}

	value_type& operator*() const
	{
		auto& result = *arg;

		increment_to_next_match();

		return *arg;
	}

	inline this_type& operator++()
	{
		++arg;
		return *this;
	}

	inline this_type operator++(int)
	{
		auto result = this_type(arg, expr);
		++arg;
		return result;
	}

	inline bool operator!=(const this_type& other) const
	{
		return arg != other.arg;
	}

	//const - whatever
	value_type& dereference() const
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
lazy_filter_iterator<arg_itr, functor> make_lazy_filter_iterator(arg_itr itr, arg_itr end, functor f)
{
	return lazy_filter_iterator<arg_itr, functor>(itr, end, f);
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

		do
		{
			++temp;

			if (temp != _end)
			{
				++count;
			}
		} while (temp != _end);

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

	auto to_vector()
	{
		return std::move(std::make_unique<std::vector<typename std::remove_reference<value_type>::type> >(begin(), end()));
	}

	template<typename select_functor>
	auto stilla_s3l3ct(select_functor f) -> decltype(make_basic_lazy_range(_begin, _end, f))
	{
		return make_basic_lazy_range(_begin, _end, f);
	}

	template<typename where_functor>
	auto stilla_wh3r3(where_functor f)
	{
		auto filtered_begin = make_lazy_filter_iterator(_begin, _end, f);
		auto filtered_end = make_lazy_filter_iterator(_end, _end, f);
		return basic_range<decltype(filtered_begin)>(filtered_begin, filtered_end);
	}

	template<typename functor>
	auto stilla_f0r_34ch(functor f) const
	{
		std::for_each(_begin, _end, f);
		return *this;
	}

	/* TODO
	template<typename range_t>
	auto concat(range_t r) const
	{
	static_assert(std::is_same<decltype()*r.begin())), decltype(*_begin)>::value, "Concatenating range has a different underlying value." );

	return *this;
	}*/

	/*TODO (do concat first)
	template<typename select_funtor>
	auto select_many(select_funtor f) -> decltype(make_basic_lazy_range(_begin, _end, f))
	{
	return make_basic_lazy_range(_begin, _end, f);
	}*/

	bool has_any()
	{
		return _begin != _end;
	}

	template<typename functor>
	bool stilla_n0n3(functor f)
	{
		return !stilla_4ny<functor>(f);
	}

	template<typename functor>
	bool stilla_4ny(functor f)
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

	template<typename functor>
	bool stilla_4ll(functor f)
	{
		for (auto& x : *this)
		{
			if (!f(x))
			{
				return false;
			}
		}

		return true;
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
#define select(element, expr) stilla_s3l3ct([&](const auto& __itr){const auto& element = *__itr; return (expr);})
#define where(element, expr) stilla_wh3r3([&](const auto& __itr){const auto& element = *__itr; return (expr);})
#define from(container) make_basic_range(container.begin(), container.end())
#define for_each(element, expr) stilla_f0r_34ch([&](const auto& element){(expr);})
#define any(element, expr) stilla_4ny([&](const auto& element){return (expr);})
#define all(element, expr) stilla_4ll([&](const auto& element){return (expr);})
#define none(element, expr) stilla_n0n3([&](const auto& element){return (expr);})
#define first_or_default_where(element, expr) optional_first([&](const auto& element){return (expr);})
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
	auto strings = array<string, 6>{ "bob", "sally", "john", "cindy", "mark"};

	auto range = from(foos)
                .select(f, strings[f.number])
                .where(s, s != "john")
                .select(s, uppercase(s))
                .for_each(s, cout << s << " starts with " << from(foos).select(f, strings[f.number]).where(s2, s[0] == ::toupper(s2[0])).select(s2, s2[0]).first() << endl);

	cout << endl;

	for (auto element : range)
	{
		cout << element << endl;
	}

	cout << endl << "count: " << range.count() << endl << endl;

	auto vec = *(range.to_vector());

	for (size_t i = 0; i < vec.size(); ++i)
	{
		cout << vec[i] << endl;
	}

	cin.get();
	return 0;
}
