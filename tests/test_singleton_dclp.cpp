#include "gtest/gtest.h"
#include <codex/singleton.hpp>

class shared_resource{
public:
  shared_resource( void ) : _counter(0){}
  ~shared_resource( void ){}

  int counter(void) {
    return ++_counter;
  }
private:
  int _counter;
};

TEST( singleton , test ){
  ASSERT_EQ( codex::singleton< shared_resource >::instance().counter() , 1 );
  ASSERT_EQ( codex::singleton< shared_resource >::instance().counter() , 2 );
}
