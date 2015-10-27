#pragma once
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

template <typename arg_itr, typename functor>
struct lazy_eval_iterator
{
	using this_type = lazy_eval_iterator<arg_itr, functor>;
	arg_itr arg;
	functor expr;

	lazy_eval_iterator(arg_itr arg_, functor expr_) : arg(arg_), expr(expr_)
	{
	}

	lazy_eval_iterator(const this_type& other) : arg(other.arg), expr(other.expr)
	{
	}

	auto operator*() const
	{
		return expr(*arg);
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
lazy_eval_iterator<arg_itr, functor> make_lazy_eval_iterator(arg_itr itr, functor f)
{
	return lazy_eval_iterator<arg_itr, functor>(itr, f);
}

template<typename itr, typename functor>
auto make_lazy_range(itr begin, itr end, functor f);