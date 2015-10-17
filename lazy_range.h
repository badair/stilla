#pragma once

#include<string>
#include<vector>
#include<list>
#include<iostream>
#include<type_traits>
#include<array>
#include<memory>
#include<boost/optional/optional.hpp>
#include<boost/optional/optional_io.hpp>
#include"lazy_iterator.h"

template <typename input_itr_type, typename transform_func>
struct lazy_range
{
private:
	using this_type = lazy_range<input_itr_type, transform_func>;

public:
	using output_itr_type = lazy_iterator<input_itr_type, transform_func>;
	using difference_type = typename std::iterator_traits<input_itr_type>::difference_type;
	const output_itr_type _begin;
	const output_itr_type _end;
	using value_type = decltype(*_begin);
	using value_type_optional = boost::optional<value_type>;

	lazy_range(input_itr_type begin, input_itr_type end, transform_func& f) : _begin(begin, f), _end(end, f)
	{
	}

	lazy_range(this_type&& other) : _begin(std::move(other._begin)), _end(std::move(other._end))
	{
	}

	value_type first() const
	{
		return *_begin;
	}

	value_type_optional optional_first() const
	{
		if (_begin < _end)
		{
			return *_begin;
		}

		return value_type_optional();
	}

	value_type last() const
	{
		return *(_end - 1);
	}

	value_type_optional optional_last() const
	{
		if (_begin < _end)
		{
			return *_begin;
		}

		return value_type_optional();
	}

	output_itr_type begin() const
	{
		return _begin;
	}

	output_itr_type end() const
	{
		return _end;
	}

	template<typename integral>
	value_type operator[](integral i) const
	{
		return _begin[i];
	}

	difference_type count() const
	{
		auto diff = _end - _begin;
		return diff > 0 ? diff : 0;
	}

	template<typename Lambda>
	const this_type& for_each(Lambda lambda) const
	{
		for (auto x : *this)
		{
			lambda(x);
		}

		return *this;
	}

	template<typename Lambda>
	this_type& for_each(Lambda lambda)
	{
		for (auto x : *this)
		{
			lambda(x);
		}

		return *this;
	}
};

template<typename input_itr_type, typename transform_func>
input_itr_type begin(const lazy_range<input_itr_type, transform_func>& r)
{
	return r._begin;
}

template<typename input_itr_type, typename transform_func>
input_itr_type end(const lazy_range<input_itr_type, transform_func>& r)
{
	return r._begin;
}

template<typename input_itr_type, typename transform_func>
lazy_range<input_itr_type, transform_func> make_lazy_range(const input_itr_type begin, const input_itr_type end, transform_func& f)
{
	return lazy_range<input_itr_type, transform_func>(begin, end, f);
}

