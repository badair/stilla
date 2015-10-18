# stilla
LINQ-inspired lazy range composition for C++14

-fluid interface
-static composition with iterators...may create a coroutine-based implementation in the future
-for now, lazy_eval_iterator implements boost::iterator_facade as a single pass traversal iterator

Example usage:
```C++
    int main()
    {
    	auto foos = vector<foo>{ foo(0), foo(1), foo(2), foo(3), foo(4) };
    	auto strings = array<string, 5>{ "bob", "sally", "john", "cindy", "mark"};
    
    	auto johnless = from(foos)
    				    .for_each(f, cout << "foo numbers: " << f.number << endl)
    				    .select(f, strings[f.number])
    				    .for_each(s, cout << "strings: " << s << endl)
    				    .where(s, s != "john");
    
    	auto john_string =   from(strings)
    						 .first_where(s, !johnless.contains(s));
    
    	cout << "john string: " << john_string << endl;
    				 
                     
    
    	cout << endl << "count: " << johnless.count() << endl << endl;
    
    	for (auto element : johnless)
    	{
    		cout << element << endl;
    	}
    
    	auto vec = johnless.to_vector();
    
    	for (auto element : vec)
    	{
    		cout << element << endl;
    	}
    
    	cout << endl;
    
    	//without macros
    	auto ugly_range = make_basic_range(foos.begin(), foos.end())
                            .select_([&](const auto& x) {return strings[(*x).number]; })
                            .where_([&](const auto& s) {return s != "john"; })
                            .select_([&](const auto& s) {return uppercase(*s); })
                            .for_each_([&](const auto& s) {cout << s << endl; });
    
    	cin.get();
    	return 0;
    }
```
-TODO: need to implement a lazy replacement for boost::filter_iterator
-TODO: template specializations, static_assert messages
-TODO: more range operations such as select_many, order_by
-TODO: const-correctness, access specifiers, move optimizations 
