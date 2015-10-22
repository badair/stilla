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

#include"range.h"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::array;
using std::string;

//you can format these macros however you prefer. The actual names are semi-mangled to prevent unintended auto-completion
#define select(element, expr) stilla_s3l3ct([&](auto element){return (expr);})
#define where(element, expr) stilla_wh3r3([&](auto element) -> bool {return (expr);})
#define from(container) make_range(begin(container), end(container))

struct foo
{
	int number;
	foo(int d) :number(d) {}
};

string uppercase(string s)
{
	for (auto& c : s)
	{
		c = toupper(c);
	}

	return s;
}

using std::begin;
using std::end;

int main()
{
	auto indexes = std::array<foo, 8>{ foo(0), foo(1), foo(2), foo(3), foo(4), foo(5), foo(6), foo(7) };
	auto names = std::vector<string>{ "bob", "sally", "sammy", "john", "cindy", "saul", "mark", "zoe" };

	auto range = 
		from(indexes)
		.select(i, names[i.number])
		.where(s, s != "john")
		.select(s, s)
	    .select(s, from(names).where(s2, s[0] == s2[0]).select(s2, s2).first());

	//lazy evaluation - the range is evaluated only when it is iterated 
	//over, which it does not do internally. "tommy" will show in the results
	//because of this. Currently, you cannot change what the original begin()
	//iterator points to (names[0] in this case).
	names[2] = "tommy";

	for (auto r : range)
	{
		cout << r << endl;
	}

	cin.get();
	return 0;
}