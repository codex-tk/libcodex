#ifndef __libcodex_function_h__
#define __libcodex_function_h__

#include <codex/codex.hpp>

namespace codex {

	/*!
		@brief lambda , function_ptr 등 호출 가능 객체 wrapper 의 선언
		@detail 템플릿 특수화를 이용하여 function< R () > 형태로 처리한다.
	*/
	template < typename Signature >
	class function;

	/*!
		@brief 호출 가능 객체의 인터페이스 선언
		@detail function<R () > 선언시 실제 타입 추론이 불가능하므로
		바인딩시 callable 의 concrete class 에 바인딩 되고 function 은
		callable 을 가지도록 구성
	*/
	template < typename Signature >
	class callable;

	/*!
		@brief callable 구현
	*/
	template < typename R, typename ...Args >
	class callable< R(Args...)> {
	public:
		callable(void);
		virtual ~callable() = default;
		virtual R operator()(Args&&... args) = 0;

		template < typename Function >
		static std::shared_ptr< callable > make_callable(Function&& f);
	};
	/*!
		@brief function 의 템플릿 특수화
		@detail Args 는 parameter pack
	*/
	template < typename R, typename ...Args >
	class function< R(Args...)>
	{
	public:
		function(void);

		/*!
		@brief 호출 가능 객체로 부터 생성
		@detail universal reference 로 추론됨
		*/
		template < typename U >
		explicit function(U&& func);
		/*!
		@brief const 호출 가능 객체로 부터 생성
		*/
		template < typename U >
		explicit function(const U& func);

		function(const function& rhs);
		function(function&& rhs);

		function& operator=(const function& rhs);
		function& operator=(function&& rhs);

		template < typename U >
		function& operator=(const U& rhs);
		template < typename U >
		function& operator=(U&& rhs);

		~function(void);

		R operator()(Args&&... args);

		void swap(function& rhs);
	private:
		std::shared_ptr < callable< R(Args...) >> _callable;
	};

	template < typename R ,  typename ...Args >
	void function< R(Args...)>::swap(function<R(Args...)>& rhs) {
		_callable.swap(rhs._callable);
	}


}

#include <codex/function.ipp>

#endif
