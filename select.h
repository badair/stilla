#pragma once

#include"lambda_wrapper.h"
#include"lazy_range.h"

#define select_from(container, itr, expr)													    \
[&]{																				            \
	using __iter_type = decltype(begin(container));												\
																				                \
	struct __lambda_iterator_wrapper {															\
		__iter_type current;																    \
		__lambda_iterator_wrapper(__iter_type start):current(start){}							\
		__lambda_iterator_wrapper(const __lambda_iterator_wrapper&) = delete;					\
	};																							\
																								\
	thread_local __lambda_iterator_wrapper __lambda_internal_iterator(begin(container));		\
    thread_local auto itr_ptr = &__lambda_internal_iterator.current;							\
																								\
	thread_local auto client_expr = wrap_lambda([&]{											\
		auto& itr = **itr_ptr;																	\
		return (expr);																			\
	});																							\
																								\
	auto range = make_lazy_range(																\
		begin(container),																		\
		end(container),																			\
		[&](__iter_type __itr) {																\
			__lambda_internal_iterator.current = __itr;											\
			return client_expr();																\
	});																							\
return std::move(range);																		\
}()