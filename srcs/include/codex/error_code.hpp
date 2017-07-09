#ifndef __libcodex_error_code_h__
#define __libcodex_error_code_h__

#include <codex/codex.hpp>

namespace codex {

	/*!
	* @brief error condition
	*/
	enum class errc {
		success = 0,
		fail = 1,
		invalid_argument = 2,
	};

	class error_category_impl
		: public std::error_category {
	public:
		virtual const char* name(void) const noexcept;
		virtual std::string message(int ev) const;
		virtual std::error_condition default_error_condition(int ev) const noexcept;
		virtual bool equivalent(const std::error_code& code, int cond) const noexcept;
		//using basic impl { return default_error_condition(code) == cond ); }
		//virtual bool equivalent( int code , const std::error_condition& cond ) const noexcept;
	};

	const std::error_category& category(void);

	std::error_code make_error_code(codex::errc ec);

	std::error_condition make_error_condition(codex::errc ec);

	std::error_code last_error(void);

}

namespace std {
	//  template <> struct is_error_code_enum< codex::errc > : public true_type{};
	template <> struct is_error_condition_enum< codex::errc > : public true_type {};
}

#endif