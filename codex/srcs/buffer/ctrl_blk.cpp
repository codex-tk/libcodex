#include <codex/buffer/ctrl_blk.hpp>

namespace codex { namespace buffer {
namespace {
  std::allocator<uint8_t> ctrl_blk_allocator;
  //codex::allocator ctrl_blk_allocator;
}

  ctrl_blk::ctrl_blk( void* p , const std::size_t sz )
    : _ref_count(1) , _ptr(p) , _size(sz)
  {
  }

  ctrl_blk::~ctrl_blk( void ) {

  }

  int ctrl_blk::add_ref( void ) {
    return _ref_count.fetch_add(1);
  }

  int ctrl_blk::release( void ) {
    int val = _ref_count.fetch_sub(1);
    if ( val == 1 ) {
      _on_destroy();
    }
    return val;
  }

  int ctrl_blk::ref_count( void ) {
    return _ref_count.load();
  }

  void* ctrl_blk::ptr( void ) {
    return _ptr;
  }

  std::size_t ctrl_blk::size( void ) const {
    return _size;
  }

  void ctrl_blk::_on_destroy( void ) {
    void* ptr = reinterpret_cast< ctrl_blk* >(_ptr) - 1;
    ctrl_blk_allocator.deallocate( reinterpret_cast< uint8_t*>(ptr)
      ,  sizeof( ctrl_blk ) + _size );
  }

  ctrl_blk* ctrl_blk::make_blk( const std::size_t sz ) {
    ctrl_blk* blk = reinterpret_cast< ctrl_blk*>(
        ctrl_blk_allocator.allocate( sizeof( ctrl_blk ) + sz ));
    new (blk) ctrl_blk( blk + 1 , sz );
    return blk;
  }

  ctrl_blk* ctrl_blk::make_blk( void* p , const std::size_t sz ){
    struct null_deleter {
      void operator()( void* ) {
      }
    };
    return make_blk( p , sz , null_deleter());
  }
/*
  void ctrl_blk::set_allocator( const codex::allocator& alloc ) {
    ctrl_blk_allocator = alloc;
  }
*/
}}
