#include "gtest/gtest.h"
#include <codex/container_of.hpp>
struct base {
	int value0;
	int value1;
};

TEST(class_member_ptr, ptr) {
	int base::* ptr = &base::value0;

	base b0;
	b0.value0 = 1;

	base* bptr = &b0;

	ASSERT_EQ(bptr->*ptr, 1);
	ASSERT_EQ(b0.*ptr, 1);
	/*
	EXPECT_EQ(bptr , nullptr);
	// 멤버 변수의 오프셋을 의미한다. bptr+mem_ptr_offset
	EXPECT_EQ(&(bptr->*ptr), nullptr);
	*/
}

TEST(find_class_ptr, container_of) {
	base obj;
	int* value1_ptr = &obj.value1;
	base* objptr = codex::container_of(value1_ptr, &base::value1 );
	ASSERT_EQ(&obj, objptr);
}
