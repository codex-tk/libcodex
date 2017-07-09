#include "gtest/gtest.h"
#include <codex/function.hpp>



TEST(create, function) {

	codex::function< int() > func([]()->int {
		return 32;
	});

	ASSERT_EQ(func(), 32);

	struct test_function {
		static double inc(int sz) {
			return sz * 1.1;
		}
	};

	codex::function< double(int) > func2(
		&test_function::inc);

	ASSERT_EQ(func2(1), 1.1);

}

TEST(callable, function) {
	
	std::shared_ptr<codex::callable< int ( int ) > > func 
		= codex::callable< int ( int ) >::make_callable(
			[](int a) -> int {
				return a;
			}
	);
	ASSERT_EQ((*func)(3), 3);
}