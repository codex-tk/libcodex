#include <codex/error_code.hpp>

namespace codex {

	const char* error_category_impl::name(void) const noexcept {
		return "codex";
	}

	std::string error_category_impl::message(int ev) const {
		static std::vector< std::string > map{
			std::string("success") ,
			std::string("fail") ,
			std::string("invalid_argument") ,
		};
		if (ev < static_cast<int>(map.size()))
			return map[ev];
		return std::string("error");
	}

	std::error_condition error_category_impl::default_error_condition(int ev) const noexcept {
		if (ev == static_cast<int>(codex::errc::invalid_argument))
			return std::make_error_condition(std::errc::invalid_argument);
		return std::error_condition(ev, *this);
	}

	bool error_category_impl::equivalent(const std::error_code& code, int cond) const noexcept {
		if (*this == code.category() && code.value() == cond) {
			return true;
		}
		if (code == std::errc::invalid_argument) {
			return cond == static_cast<int>(codex::errc::invalid_argument);
		}
		return false;
	}

	/*
	bool error_category_impl::equivalent( int code , const std::error_condition& cond ) const noexcept {
	if ( *this == cond.category() && cond.value() == code ) {
	return true;
	}
	if ( code == static_cast<int>(codex::errc::invalid_argument) ) {
	return cond == std::errc::invalid_argument;
	}
	return false;
	}
	*/
	const std::error_category& category(void) {
		static error_category_impl impl;
		return impl;
	}

	std::error_code make_error_code(codex::errc ec) {
		return std::error_code(static_cast<int>(ec), codex::category());
	}

	std::error_condition make_error_condition(codex::errc ec) {
		return std::error_condition(static_cast<int>(ec), codex::category());
	}

	std::error_code last_error(void) {
#if defined( __codex_win32__)
		return std::error_code(GetLastError(), std::system_category());
#else
		return std::error_code(errno, std::system_category());
#endif
	}

}

namespace std {

}