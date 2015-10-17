#pragma once

template<class Lambda>
class lambda_wrapper
{
	Lambda exec;

public:

	lambda_wrapper(Lambda &&t) : exec(std::forward<Lambda>(t))
	{
	}

	//copy not allowed
	lambda_wrapper(const lambda_wrapper<Lambda>& t) = delete;

	//move allowed
	lambda_wrapper(lambda_wrapper<Lambda>&& t) : exec(std::move(t.exec))
	{
	}

	auto operator()() -> decltype(exec())
	{
		return exec();
	}
};

template<class Lambda>
lambda_wrapper<Lambda> wrap_lambda(Lambda &&t)
{
	return std::forward<Lambda>(t);
}

