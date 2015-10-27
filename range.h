
#include "lazy_eval_iterator.h"
#include "lazy_filter_iterator.h"
#include "parallel_generator.h"
#include "lock_free_parallel_generator.h"

template<typename itr>
struct range
{
	using this_type = range<itr>;

	using iterator = itr;

	itr _begin;
	itr _end;

	using value_type = typename std::remove_reference<typename std::remove_pointer<itr>::type>::type;

	auto begin()
	{
		return _begin;
	}

	auto end()
	{
		return _end;
	}

	auto first()
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

	template<typename select_functor>
	auto stilla_s3l3ct(select_functor f)
	{
		return make_lazy_range(_begin, _end, f);
	}

	template<typename where_functor>
	auto stilla_wh3r3(where_functor f)
	{
		return range<decltype(make_lazy_filter_iterator(_begin, _end, f))>(make_lazy_filter_iterator(_begin, _end, f), make_lazy_filter_iterator(_end, _end, f));
	}

	template<typename functor>
	auto stilla_f0r_34ch(functor f) const
	{
		std::for_each(_begin, _end, f);
		return *this;
	}

	/* TODO works, but is hacky
	template <typename V = value_type>
	typename std::enable_if<
		!std::is_trivially_copyable<V>::value || 
		!std::is_trivially_destructible<V>::value,	
	parallel_generator<itr> >::type
	parallel()
	{
		return make_parallel_generator(_begin, _end);
	}*/

	template <typename V = value_type>
	typename std::enable_if<
		true,//std::is_scalar<V>::value
		     //|| (std::is_trivially_copyable<V>::value && std::is_trivially_destructible<V>::value),
	lock_free_parallel_generator<itr> >::type
	parallel()
	{
		return make_lock_free_parallel_generator(_begin, _end);
	}

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

	range(itr begin_, itr end_) : _begin(begin_), _end(end_)
	{
	}

	range(const this_type& other) : _begin(other._begin), _end(other._end)
	{
	}

	range(this_type&& other) : _begin(std::forward<decltype(other._begin)>(other._begin)), _end(std::forward<decltype(other._end)>(other._end))
	{
	}
};

template<typename itr, typename functor>
auto make_lazy_range(itr begin, itr end, functor f)
{
	using lazy_itr = decltype(make_lazy_eval_iterator(begin, f));
	return range<lazy_itr>(make_lazy_eval_iterator(begin, f), make_lazy_eval_iterator(end, f));
}

template<typename itr>
auto inline make_range(itr&& begin, itr&& end)
{
	return range<itr>(std::forward<itr>(begin), std::forward<itr>(end));
}

template<typename container>
auto inline make_range(container& c) {
	return make_range(begin(c), end(c));
}