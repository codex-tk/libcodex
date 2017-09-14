/**
 */

#ifndef __codex_buffer_ctrl_blk_h__
#define __codex_buffer_ctrl_blk_h__

#include <codex/codex.hpp>


namespace codex { namespace buffer {

  class ctrl_blk{
  protected:
    ctrl_blk( void* p , const std::size_t sz );
  private:
    virtual ~ctrl_blk( void );
  public:
    int add_ref( void );
    int release( void );
    int ref_count( void );

    void* ptr( void );
    std::size_t size( void ) const ;

    virtual void _on_destroy( void );
  public:
    static ctrl_blk* make_blk( const std::size_t sz );
    static ctrl_blk* make_blk( void* ptr , const std::size_t sz );
    template < class Deleter >
    static ctrl_blk* make_blk( void* ptr , const std::size_t sz , Deleter&& d );
    
  private:
    std::atomic<int> _ref_count;
    void* _ptr;
    std::size_t _size;
  };

  template < class Deleter >
  ctrl_blk* ctrl_blk::make_blk( void* ptr , const std::size_t sz , Deleter&& d ) {
    class ctrl_blk0 : public ctrl_blk{
    public:
      ctrl_blk0( void* p , const std::size_t sz , Deleter&& d )
        : ctrl_blk( p , sz )
        , _deleter(std::forward<Deleter>(d))
      {
      }
      virtual ~ctrl_blk0(void) {

      }
      virtual void _on_destroy( void ) {
        _deleter( ptr() );
        delete this;
      }
    private:
      Deleter _deleter;
    };
    return new ctrl_blk0( ptr , sz , std::forward<Deleter>(d) );
  }

}}

#endif
