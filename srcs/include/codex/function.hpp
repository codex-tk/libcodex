#ifndef __libcodex_function_h__
#define __libcodex_function_h__

#include <codex/codex.hpp>

namespace codex {

	/**
		@brief lambda , function_ptr �� ȣ�� ���� ��ü wrapper �� ����
		@detail ���ø� Ư��ȭ�� �̿��Ͽ� function< R () > ���·� ó���Ѵ�.
	*/
	template < typename Signature >
	class function;

	/**
		@brief ȣ�� ���� ��ü�� �������̽� ����
		@detail function<R () > ����� ���� Ÿ�� �߷��� �Ұ����ϹǷ�
		���ε��� callable �� concrete class �� ���ε� �ǰ� function �� 
		callable �� �������� ����
	*/
	template < typename Signature >
	class callable;

	/**
		@brief callable ����
	*/
	template < typename R, typename ...Args >
	class callable< R (Args...)> {
	public:
		callable(void);
		virtual ~callable() = default;
		virtual R operator()(Args&&... args) = 0;

		template < typename Function >
		static std::shared_ptr< callable > make_callable(Function&& f);
	};
	/**
		@brief function �� ���ø� Ư��ȭ
		@detail Args �� parameter pack
	*/
	template < typename R , typename ...Args >
	class function< R (Args...)>
	{
	public:
		function( void );
		
		/**
			@brief ȣ�� ���� ��ü�� ������
		*/
		template < typename U >
		explicit function(U&& func);

		~function( void );

		R operator()(Args&&... args);
	private:
		std::shared_ptr < callable< R(Args...) >> _callable;
	};



}

#include <codex/function.ipp>

#endif