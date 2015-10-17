#include<string>
#include<vector>
#include<list>
#include<iostream>
#include<type_traits>
#include<array>
#include<memory>
#include<iterator>
#include<functional>
#include<utility>

#include"select.h"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;

class foo
{
	const int number;
public:

	int get_number()
	{
		return number;
	}

	foo(int d)
		:number(d)
	{
	}
}; 

template<typename range_t>
void print_range_data(const range_t& range, const char* tag)
{
	cout << endl <<  "   ********* " << tag << " *********";
	auto count = range.count();
	cout << endl << "count: " << count << endl;
	cout << endl << "optional_first: " << range.optional_first() << endl;
	cout << endl << "optional_last: " << range.optional_last() << endl;

	cout << endl << "for_each: " << endl;
	range.for_each([&](auto i) {cout << i << endl; });

	cout << endl << "range-based for: " << endl;

	for (auto cur : range)
	{
		cout << cur << endl;
	}

	cout << endl;

	for (auto i = 0; i < count; ++i)
	{
		cout << "operator[" << i << "]: " << range[i] << endl;
	}

	cout << endl << "first: " << range.first() << endl;
	cout << endl << "last: " << range.last() << endl;

	cout << endl;
	cout << endl;
}

int main()
{
	{
		auto foos = std::vector<foo>{ foo(1) ,foo(2) ,foo(3),foo(4),foo(5) };
		auto strings = std::array<string, 5>{ "abc", "def", "ghi", "jkl", "mno"};
		int offset = 1;
		auto range = select_from(foos, temp_foo, strings[temp_foo.get_number() - offset]);
		print_range_data(range, "selecting from foos");
	}

	{
		auto empty_vec = std::vector<string>{};
		auto range = select_from(empty_vec, x, &x);
		//print_range_data(range, "selecting from empty range");
	}

	cin.get();
	return 0;
}
