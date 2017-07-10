#include "gtest/gtest.h"
#include <codex/codex.hpp>
#include <codex/error_code.hpp>

TEST(diag, error) {
	std::error_code ec = codex::make_error_code(
		codex::errc::fail);
	ASSERT_TRUE(ec);
	// compare error_code && error_condtion
	/*
	namespace std{
		template <> struct is_error_condition_enum< codex::errc > : public true_type {};
	}
	*/
	// 선언시 codex::errc->error_condtion 으로 변환 수행
	ASSERT_EQ(ec, codex::errc::fail);
	ASSERT_NE(ec, codex::errc::success);
}

TEST(diag, error_condition) {
	std::error_code ec = codex::make_error_code(
		codex::errc::invalid_argument);
	std::error_condition ec2 = codex::make_error_condition(codex::errc::invalid_argument);

	// ==( std::error_code , std::error_condition )
	ASSERT_EQ(ec, ec2);
	// ==( std::error_code , std::error_condition )
	// std::errc->std:error_condition
	ASSERT_EQ(ec, std::errc::invalid_argument);
	// ==( std::error_condition , std::error_code )
	ASSERT_EQ(ec2, std::make_error_code(std::errc::invalid_argument));
}
