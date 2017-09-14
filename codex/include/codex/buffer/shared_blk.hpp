/**
 */
#ifndef __codex_buffer_shared_blk_h__
#define __codex_buffer_shared_blk_h__

#include <codex/buffer/ctrl_blk.hpp>

namespace codex { namespace buffer {

  class shared_blk {
  public:
    shared_blk( void );
    shared_blk( const shared_blk& rhs );
    shared_blk( shared_blk&& rhs );
    shared_blk( const std::size_t sz );
    shared_blk( void* buf , const std::size_t sz );
    template < class Deleter >
    shared_blk( void* buf , const std::size_t sz , Deleter&& d );

    shared_blk& operator=( const shared_blk& blk );
    shared_blk& operator=( shared_blk&& blk );

    ~shared_blk( void );

    int ref_count( void );

    /**
     * total buffer size
     */
    std::size_t size( void ) const;

    void* read_ptr( void );
    int read_skip( const int move );
    /**
     * readable data length
     */
    std::size_t length( void ) const;

    void* write_ptr( void );
    int write_skip( const int move );

    /**
     * writable buffer space
     */
    std::size_t space( void ) const;

    void clear( void );

    void swap( shared_blk& blk );

    /**
     * if ( blk.space() < sz ) {
     *   blk = buffer::reserve( blk.length() + sz );
     * }
     * blk.write( p , sz );
     *
     * or
     *
     * if ( blk.space() < sz ) {
     *   std::size_t sz0 = blk.length() + sz;
     *   shared_blk nblk( some_alloc( sz0 ) , sz0 , [] ( void* ptr ) {
     *     some_deleter(ptr);
     *   });
     *   nblk.write( blk.read_ptr() , blk.length());
     *   nblk.swap(blk);
     * }
     * blk.write( p , sz );
     */
    int write( void* p , int sz );
    int read( void* p , int sz );

    void compact( void );
  private:
    void add_ref( void );
    void release( void );

  private:
    codex::buffer::ctrl_blk* _blk;
    /*
     * 0 <= _read <= _write <= size()
     */
    std::size_t _read;
    std::size_t _write;
  };

  template < class Deleter >
  shared_blk::shared_blk( void* buf , const std::size_t sz , Deleter&& d )
    : _blk( codex::buffer::ctrl_blk::make_blk( buf , sz , std::forward< Deleter >(d)))
    , _read(0) , _write(0)
  {
  }

  shared_blk reserve( shared_blk blk , const int sz );

}}

#endif
