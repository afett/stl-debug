/*

	Copyright (c) 2019 Andreas Fett
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice, this
	  list of conditions and the following disclaimer.

	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	see https://gcc.gnu.org/onlinedocs/libstdc%2B%2B/manual/debug_mode.html
*/

#define _GLIBCXX_DEBUG
#include <vector>
#include <iostream>
#include <map>
#include <algorithm>

/*
/usr/include/c++/6/debug/vector:415:
Error: attempt to subscript container with out-of-bounds index 0, but 
container only holds 0 elements.

Objects involved in the operation:
    sequence "this" @ 0x0x7ffe98a79af0 {
      type = std::__debug::vector<int, std::allocator<int> >;
    }
*/
void out_of_bounds()
{
	std::vector<int> v{1, 2, 3, 4};
	std::cout << v[5];
}

void out_of_bounds_addr()
{
	std::vector<int> v{};
	std::cout << &v[0];
}

/*
/usr/include/c++/6/debug/safe_iterator.h:375:
Error: attempt to advance a dereferenceable iterator 4 steps, which falls 
outside its valid range.

Objects involved in the operation:
    iterator @ 0x0x7ffefb38ddb0 {
      type = __gnu_debug::_Safe_iterator<__gnu_cxx::__normal_iterator<int*, std::__cxx1998::vector<int, std::allocator<int> > >, std::__debug::vector<int, std::allocator<int> > > (mutable iterator);
      state = dereferenceable;
      references sequence with type 'std::__debug::vector<int, std::allocator<int> >' @ 0x0x7ffefb38dde0
    }
*/
void iterator_out_of_range()
{
	std::vector<int> v{0, 0, 0, 0};
	auto pos = std::begin(v) + 2;
	auto space = std::distance(pos + 4, end(v));
	std::cerr << space;
}

/*
/usr/include/c++/6/bits/stl_algo.h:4737:
Error: comparison doesn't meet irreflexive requirements, assert(!(a < a)).

Objects involved in the operation:
    instance "functor" @ 0x0x7fffba7db5d0 {
      type = comparison_not_irreflexive()::{lambda(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)#1};
    }
    iterator::value_type "ordered type" {
      type = std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >;
    }
*/
void comparison_not_irreflexive()
{
	std::vector<std::string> v{"foo", "bar", "baz"};
	std::sort(begin(v), end(v), [](std::string const& a, std::string const& b) {
				return a <= b;
			});
}

/*
/usr/include/c++/6/bits/stl_algo.h:5309:
Error: elements in iterator range [__first1, __last1) are not sorted 
according to the predicate __comp.

Objects involved in the operation:
    iterator "__first1" @ 0x0x7ffe5d4b9060 {
      type = __gnu_debug::_Safe_iterator<__gnu_cxx::__normal_iterator<item*, std::__cxx1998::vector<item, std::allocator<item> > >, std::__debug::vector<item, std::allocator<item> > > (mutable iterator);
      state = dereferenceable (start-of-sequence);
      references sequence with type 'std::__debug::vector<item, std::allocator<item> >' @ 0x0x7ffe5d4b8e90
    }
    iterator "__last1" @ 0x0x7ffe5d4b9030 {
      type = __gnu_debug::_Safe_iterator<__gnu_cxx::__normal_iterator<item*, std::__cxx1998::vector<item, std::allocator<item> > >, std::__debug::vector<item, std::allocator<item> > > (mutable iterator);
      state = past-the-end;
      references sequence with type 'std::__debug::vector<item, std::allocator<item> >' @ 0x0x7ffe5d4b8e90
    }
*/
struct item {
	int id;
	std::string name;
};

void diff_not_sorted()
{
	auto item_cmp = [](item const& a, item const& b) { return a.id < b.id; };

	std::vector<item> items{{2, "bar"}, {3, "baz"}, {1, "foo"}};
	// std::sort(begin(items), end(items), item_cmp);
	std::vector<item> update{{1, "foo"}, {4, "boom"} ,{2, "bang"}};
	// std::sort(begin(update), end(update), item_cmp);

	decltype(items) deleted;
	std::set_difference(begin(items), end(items), begin(update), end(update),
		std::back_inserter(deleted), item_cmp);

	for (auto item: deleted) {
		std::cerr << "d: {" << item.id << ", " << item.name << "}\n";
	}

	decltype(items) added;
	std::set_difference(begin(update), end(update), begin(items), end(items),
		std::back_inserter(added), item_cmp);

	for (auto item: added) {
		std::cerr << "a: {" << item.id << ", " << item.name << "}\n";
	}
}

void usage(std::map<std::string, void(*)()> const& fn)
{
	std::cerr << "argument must be one of:\n";
	for (auto entry: fn) {
		std::cerr << entry.first << "\n";
	}
}

bool run(std::string const& cmd, std::map<std::string, void(*)()> const& fn)
{
	for (auto entry: fn) {
		if (entry.first == cmd) {
			entry.second();
			return true;
		}
	}
	return false;
}

int main(int argc, char *argv[])
{
#define FN_ENTRY(name) { #name, &name }
	std::map<std::string, void(*)()> fn = {
		FN_ENTRY(out_of_bounds),
		FN_ENTRY(out_of_bounds_addr),
		FN_ENTRY(iterator_out_of_range),
		FN_ENTRY(comparison_not_irreflexive),
		FN_ENTRY(diff_not_sorted),
	};
#undef FN_ENTRY

	if (argc == 2 && run(argv[1], fn)) {
		return 0;
	}

	usage(fn);
	return 1;
}
