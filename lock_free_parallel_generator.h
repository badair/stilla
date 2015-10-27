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
#include<future>
#include <boost/lockfree/queue.hpp>

template <typename itr>
struct lock_free_parallel_generator
{
	using this_type = lock_free_parallel_generator<itr>;
	using value_type = typename std::remove_reference<decltype(*std::declval<itr>())>::type;
	using queue = boost::lockfree::queue<value_type>;

	std::shared_ptr<queue> result_queue;
	itr end_itr;
	itr current_itr;

	lock_free_parallel_generator(itr begin_, itr end_) : current_itr(begin_), end_itr(end_), result_queue(std::make_shared<queue>(128))
	{
	}

	lock_free_parallel_generator operator=(this_type& other){
		result_queue = other.result_queue;
		end_itr = other.end_itr;
		current_itr = other.current_itr;
	}

	auto operator*()
	{
		value_type value;

		//spin until a result appears in the queue
		while (!result_queue->pop(value));

		return value;
	}

	auto begin()
	{
		auto cur = current_itr;

		while (cur != end_itr)
		{
			std::thread([&, cur = cur] {
				result_queue->push(*cur);
			}).detach();

			++cur;
		}

		return *this;
	}

	auto end()
	{
		return *this;
	}

	inline this_type& operator++()
	{
		++current_itr;
		return *this;
	}

	inline bool operator!=(const this_type& other) const
	{
		return current_itr != end_itr;
	}
};

template <typename itr>
lock_free_parallel_generator<itr> make_lock_free_parallel_generator(itr begin, itr end)
{
	return lock_free_parallel_generator<itr>(begin, end);
}