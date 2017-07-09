#ifndef __libcodex_function_h__
#define __libcodex_function_h__

#include <codex/codex.hpp>

namespace codex {

	/*!
		@brief lambda , function_ptr �� ȣ�� ���� ��ü wrapper �� ����
		@detail ���ø� Ư��ȭ�� �̿��Ͽ� function< R () > ���·� ó���Ѵ�.
	*/
	template < typename Signature >
	class function;

	/*!
		@brief ȣ�� ���� ��ü�� �������̽� ����
		@detail function<R () > ����� ���� Ÿ�� �߷��� �Ұ����ϹǷ�
		���ε��� callable �� concrete class �� ���ε� �ǰ� function ��
		callable �� �������� ����
	*/
	template < typename Signature >
	class callable;

	/*!
		@brief callable ����
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
		@brief function �� ���ø� Ư��ȭ
		@detail Args �� parameter pack
	*/
	template < typename R, typename ...Args >
	class function< R(Args...)>
	{
	public:
		function(void);

		/*!
		@brief ȣ�� ���� ��ü�� ���� ���� 
		@detail universal reference �� �߷е�
		*/
		template < typename U >
		explicit function(U&& func);
		/*!
		@brief const ȣ�� ���� ��ü�� ���� ����
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