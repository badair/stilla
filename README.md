# stilla
lazy/parallel range composition for C++14 

-proxy iterators used to achieve lazy evaluation
-allows a lambda "expression" shorthand syntax akin to C#
-fluid interface via unobtrusive macros (actual function names are semi-mangled to avoid unwanted autocompletion)

```C++
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
#include<chrono>
#include<cstring>

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

string slow_uppercase(string s)
{
	string result = "";

	for (auto c : s)
	{
		result += toupper(c);

		for (int i = 0; i < 9999999; i++);
	}

	return result;
}

using std::begin;
using std::end;

int main()
{
	auto silly_indexes = std::list<foo>{ foo(0), foo(1), foo(2), foo(3), foo(4), foo(5), foo(6), foo(7), foo(8), foo(9), foo(10)};
	auto letters = std::vector<char>{ 'j', 'a', 'b', 'c', 'd', 'f', 'h', 'i', 'm', 's', 'z' };
	auto names = std::array<string, 11>{ "johnjacobjingleheimerschmidt", "allen", "bob", "cindy", "derek", "hannah", "ian", "megan", "stewart", "zoe", "freddy"};

	auto my_range =
		from(silly_indexes)
		.select(f, letters[f.number])
		.where(l, l != 'd')
		.select(l, from(names).where(n, n[0] == l).first())
		.where(n, n != "megan" && n != "alex" && n.size() > 3)
		.select(n, slow_uppercase(n));

	names[1] = "alex";

	cout << "lazy evaluation..." << endl;

	for (auto element : my_range)
	{
		cout << element << endl;
	}

	cout << endl << "parallel evaluation/iteration..." << endl;

	for (auto element : my_range.parallel())
	{
		cout << element << endl;
	}

	cout << endl << "press key to exit..." << endl;
	cin.get();
	return 0;
}

/***sample output**

	lazy evaluation...
	JOHNJACOBJINGLEHEIMERSCHMIDT
	CINDY
	FREDDY
	HANNAH
	STEWART
	
	parallel evaluation/iteration...
	CINDY
	FREDDY
	HANNAH
	STEWART
	JOHNJACOBJINGLEHEIMERSCHMIDT
	
	press key to exit...
*/
```

-TODO: 
-boost unit tests
-more detailed template specializations for different kinds of containers/iterators
-thorough static_assert messages
-fully implement standard iterator interfaces
-many more range operations: select_many, concat, order_by, zip, select overloads
-range mutability within lambda
-do away with macros?
-better cv correctness, access restrictions
-constexpr
-fix tabs in whitespace






