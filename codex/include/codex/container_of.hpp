#ifndef __libcodex_container_of_h__
#define __libcodex_container_of_h__

#include <codex/codex.hpp>

namespace codex {

	/*!
	@brief 멤버 변수 포인터로 부터 부모 클래스 주소를 알아오는 함수
	@detail Member Parent::* ptr 은 &Class::Member 를 의미함
	*/
	template < typename Parent , typename Member >
	Parent* container_of0(Member* ptr , Member Parent::*mem_ptr) {
		Parent* parent = nullptr;
		std::ptrdiff_t val = reinterpret_cast<std::ptrdiff_t>(&(parent->*mem_ptr));
		val = reinterpret_cast<std::ptrdiff_t>(ptr) - val;
		return reinterpret_cast<Parent*>(val);
	}

	// from boost::intrinsic
	template<class Parent, class Member>
	std::ptrdiff_t offset_of(const Member Parent::*ptr_to_member) {
		const Parent * const parent = nullptr;
		const char *const member = static_cast<const char*>(
			static_cast<const void*>(&(parent->*ptr_to_member)));
		std::ptrdiff_t val(member - static_cast<const char*>(
			static_cast<const void*>(parent)));
		return val;
	}

	template<class Parent, class Member>
	inline Parent* container_of(Member *member, const Member Parent::* ptr_to_member)
	{
		return static_cast<Parent*>(
			static_cast<void*>(
				static_cast<char*>(
					static_cast<void*>(member)) - offset_of(ptr_to_member)
				)
			);
	}

	template<class Parent, class Member>
	inline const Parent *container_of(const Member *member, const Member Parent::* ptr_to_member)
	{
		return static_cast<const Parent*>(
			static_cast<const void*>(
				static_cast<const char*>(static_cast<const void*>(member)) - offset_of(ptr_to_member)
				)
			);
	}

}

#endif
