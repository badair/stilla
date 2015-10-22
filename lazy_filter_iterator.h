#include<string>
#include<vector>
#include<list>
#include<iostream>
#include<fstream>
#include<type_traits>
#include<array>
#include<memory>
#include<cassert>
#include<list>
#include<algorithm>

template <typename arg_itr, typename functor>
struct lazy_filter_iterator
{
	using expr_type = functor;
	using this_type = lazy_filter_iterator<arg_itr, functor>;

	arg_itr arg;
	arg_itr end;
	functor expr;

	using value_type = typename std::remove_pointer<arg_itr>::type;

	lazy_filter_iterator(arg_itr arg_, arg_itr end_, functor expr_) : arg(arg_), end(end_), expr(expr_)
	{
	}

	lazy_filter_iterator(const this_type& other) : arg(other.arg), end(other.end), expr(other.expr)
	{
	}

	void increment_to_next_match()
	{
		while (arg != end && !expr(*arg))
		{
			++arg;
		}
	}

	auto operator*()
	{
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
};

template <typename arg_itr, typename functor>
lazy_filter_iterator<arg_itr, functor> make_lazy_filter_iterator(arg_itr itr, arg_itr end, functor f)
{
	return lazy_filter_iterator<arg_itr, functor>(itr, end, f);
}



