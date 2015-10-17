#pragma once

#include<type_traits>
#include<iterator>

template<typename input_itr_type, typename transform_func>
class lazy_iterator : std::random_access_iterator_tag
{
private:
	transform_func& transform;
	input_itr_type input;

	
	using underlying_type = typename std::remove_pointer<input_itr_type>::type;
	using this_type = lazy_iterator<input_itr_type, transform_func>;

public:

	using value_type = typename std::result_of<transform_func(input_itr_type)>::type;
	using difference_type = typename std::iterator_traits<input_itr_type>::difference_type;
	using reference = typename std::add_lvalue_reference<value_type>::type;
	using iterator_category = typename std::iterator_traits<input_itr_type>::iterator_category;

	lazy_iterator(input_itr_type in, transform_func& lam) : transform(lam), input(in)
	{
	}

	lazy_iterator(const this_type& other) : transform(other.transform), input(other.input)
	{
	}

	lazy_iterator(this_type&& other) : transform(other.transform), input(other.input)
	{
	}

	template<typename integral>
	inline this_type& operator -=(integral n)
	{
		return (*this) += -n;
	}

	template<typename integral>
	inline this_type& operator +=(integral n)
	{
		if (n >= 0)
		{
			while (n--) ++input;
		}
		else while (n++)
		{
			--input;
		}

		return *this;
	}

	inline value_type operator*() const
	{
		return transform(input);
	}

	inline this_type& operator++()
	{
		++input;
		return *this;
	}

	inline this_type& operator--()
	{
		--input;
		return *this;
	}

	inline this_type operator--(int)
	{
		auto result = this_type(input, transform);
		--input;
		return result;
	}

	inline this_type operator++(int)
	{
		auto result = this_type(input, transform);
		++input; 
		return result;
	}

	template<typename integral>
	inline this_type operator+(integral i) const
	{
		auto temp = input;
		temp += i;
		return{ temp, transform };
	}

	template<typename integral>
	inline this_type operator-(integral i) const
	{
		return (*this) + -i;
	}

	inline difference_type operator+(this_type i) const
	{
		return input + i.input;
	}

	inline difference_type operator-(this_type i) const
	{
		return input - i.input;
	}

	template<typename integral>
	inline value_type operator[](integral i) const
	{
		return transform(input + i);
	}

	bool operator!=(this_type other) const
	{
		//heavy use of this overload will bloat the executable
		return input != other.input;
	}

	bool operator<(this_type other) const
	{
		//heavy use of this overload will bloat the executable
		return input < other.input;
	}

	bool operator>(this_type other) const
	{
		//heavy use of this overload will bloat the executable
		return input > other.input;
	}
};