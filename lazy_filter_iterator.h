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

	arg_itr current;
	arg_itr end;
	functor expr;

	using value_type = typename std::remove_pointer<arg_itr>::type;

	lazy_filter_iterator(arg_itr arg_, arg_itr end_, functor expr_) : current(arg_), end(end_), expr(expr_)
	{
	}

	lazy_filter_iterator(const this_type& other) : current(other.current), end(other.end), expr(other.expr)
	{
	}

	auto operator*() const
	{
		auto temp = current;

		while (temp != end && !expr(*temp))
		{
			++temp;
		}

		return *temp;
	}

	inline this_type& operator++()
	{
		++current;

		while (current != end && !expr(*current))
		{
			++current;
		}

		return *this;
	}

	inline bool operator!=(const this_type& other) const
	{
		return current != other.current;
	}
};

template <typename arg_itr, typename functor>
lazy_filter_iterator<arg_itr, functor> make_lazy_filter_iterator(arg_itr itr, arg_itr end, functor f)
{
	return lazy_filter_iterator<arg_itr, functor>(itr, end, f);
}



