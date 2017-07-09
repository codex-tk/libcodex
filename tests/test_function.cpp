#include "gtest/gtest.h"
#include <codex/function.hpp>

TEST(create , function) {
	codex::function< int() > func([]()->int {
		return 32;
	});
	ASSERT_EQ(func(), 32);

	codex::function< int() > func_assigned(func);

	ASSERT_EQ(func(), func_assigned());

	func_assigned = std::move(func);

	ASSERT_NE(func(), func_assigned());

	const auto f0 = []() -> int {
		return 42;
	};
	codex::function< int() > func_from_const_u(f0);
	ASSERT_EQ(func_from_const_u(), 42);
}

TEST(assign0, function) {

	codex::function< int() > func([]()->int {
		return 32;
	});
	ASSERT_EQ(func(), 32);

	// rvalue assign
	func = []() -> int {
		return 41;
	};

	ASSERT_EQ(func(), 41);
	// lvalue assign
	// const 없이는 U&& 로 호출된다 -> 타입추론
	const auto lvalfunc = []() -> int {
		return 42;
	};

	func = lvalfunc;

	ASSERT_EQ(func(), 42);

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