# stilla
LINQ-inspired lazy range composition for C++14

-fluid interface using unobtrusive macros with a lambda "expression" shorthand syntax akin to C#

-iterator dereference operator used to achieve statically dispatched lazy evaluation (may create a coroutine-based implementation in the future)

Example usage:
```C++
struct foo
{
	const int number;
	foo(int d) :number(d) {}
};

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

/*output
bob
sally
tommy
cindy
sally
mark
zoe
*/
```
Why the name "Stilla"? I don't know.

-TODO: template specializations, static_assert messages

-TODO: more range operations: select_many, concat, order_by

-TODO: better const-correctness, access specifiers







