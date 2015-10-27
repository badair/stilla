#include<list>
#include<memory>
#include<future>
#include<chrono>

#pragma once

template <typename itr>
struct parallel_generator
{
	using this_type = parallel_generator<itr>;
	using value_type = typename std::remove_reference<decltype(*std::declval<itr>())>::type;
	using result_list = std::vector<std::pair<std::future<value_type>, bool > >;

	itr end_itr;
	itr current_itr;
	std::shared_ptr<result_list> results;

	parallel_generator(itr begin_, itr end_) : current_itr(begin_), end_itr(end_), results(std::make_shared<result_list>())
	{
	}

	parallel_generator operator=(this_type& other) {
		results = other.results;
		end_itr = other.end_itr;
		current_itr = other.current_itr;
	}

	auto operator*()
	{
		int index = 0;
		int size = results->size();
		auto& results_vec = *results;

		//todo needs lots of clean up 
		while (results_vec[index % size].second 
			|| (!results_vec[index % size].first.valid() 
			|| (results_vec[index % size].first.wait_for(std::chrono::seconds(0)) != std::future_status::ready)))
		{
			index++;
		}

		results_vec[index % size].second = true;
		return results_vec[index % size].first.get();
	}

	auto begin()
	{
		auto current = current_itr;

		while (current != end_itr)
		{
			results->push_back(std::make_pair(std::move(std::async([&, cur = current] {
				return *cur; 
			})), false));

			++current;
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
		return current_itr != other.end_itr;
	}
};

template <typename itr>
parallel_generator<itr> make_parallel_generator(itr begin, itr end)
{
	return parallel_generator<itr>(begin, end);
}
