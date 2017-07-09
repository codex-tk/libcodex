namespace codex{

	template < typename R, typename ...Args >
	callable< R(Args...)>::callable(void)
	{
	}

	template < typename R, typename ...Args >
	template < typename Function >
	std::shared_ptr< callable< R(Args...)> >
		callable< R(Args...) >::make_callable( Function&& func )
	{
		class _callable : public callable {
		public:
			_callable(Function&& f) : _function(std::forward< Function >(f)) {}
			virtual ~_callable(void) {}
			virtual R operator()(Args&&... args) {
				return _function(std::forward< Args >(args)...);
			}
		private:
			Function _function;
		};
		return std::make_shared<_callable>( std::forward<Function>(func) );
	}

	template < typename R, typename ...Args >
	function< R(Args...)>::function(void)
	{
	}


	template < typename R, typename ...Args >
	function< R(Args...)>::~function(void)
	{
	}
	
	template < typename R, typename ...Args >
	template < typename U >
	function< R(Args...)>::function(U&& func) 
		: _callable(callable< R(Args...)>::make_callable(
		std::forward<U>(func)))
	{

	}


	template < typename R, typename ...Args >
	R function< R(Args...)>::operator()(Args&&... args)
	{
		if( _callable ) 
			return (*_callable)( std::forward<Args>(args)...);
		return R();
	}
}