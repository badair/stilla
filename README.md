# stilla
LINQ-inspired lazy range composition for C++14

-fluid interface via unobtrusive macros - create lambdas using syntax similar to C#

-static composition built on iterator dereferencing (may create a coroutine-based implementation in the future)

-for now, implements boost::iterator_facade as a single pass iterator

Example usage:
```C++
struct foo
{
	const int number;
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

    int main()
{
	auto foos = vector<foo>{ foo(0), foo(1), foo(2), foo(3), foo(4) };
	auto strings = array<string, 6>{ "bob", "sally", "john", "cindy", "mark"};

	auto range = from(foos)
                .select(f, strings[f.number])
                .where(s, s != "john")
                .select(s, uppercase(s))
                .for_each(s, 
                     cout << s << " starts with " 
                     << from(foos)
                        .select(f, strings[f.number])
                        .where(s2, s[0] == ::toupper(s2[0]))
                        .select(s2, s2[0])
                        .first() 
                    << endl
                );

	cout << endl;

	for (auto element : range)
	{
		cout << element << endl;
	}

	cout << endl << "count: " << range.count() << endl << endl;

	auto vec = *(range.to_vector());

	for (size_t i = 0; i < vec.size(); ++i)
	{
		cout << vec[i] << endl;
	}

	cin.get();
	return 0;
}

/*output
BOB starts with b
SALLY starts with s
CINDY starts with c
MARK starts with m

BOB
SALLY
CINDY
MARK

count: 4

BOB
SALLY
CINDY
MARK
*/
```

-TODO: template specializations, static_assert messages

-TODO: compound range operations such as select_many, concat

-TODO: const-correctness, access specifiers, possibly move optimizations 

-TODO: order_by?



