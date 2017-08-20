#ifndef __libcodex_singleton_h__
#define __libcodex_singleton_h__

#include <codex/codex.hpp>
#include <atomic>

namespace codex {

  template < typename typeT >
  class singleton {
  public:
    singleton() = delete;
    ~singleton() = delete;

    static typeT& instance( void ) {
      typeT* instance = _instance.load( std::memory_order_relaxed );
      std::atomic_thread_fence(std::memory_order_acquire);
      if ( instance == nullptr ) {
        std::lock_guard<std::mutex> guard(_lock);
        instance = _instance.load( std::memory_order_relaxed );
        if ( instance == nullptr ) {
          instance = new typeT();
          std::atomic_thread_fence(std::memory_order_release);
          _instance.store( instance , std::memory_order_relaxed );
        }
      }
      return *instance;
    }
  private:
    static std::mutex _lock;
    static std::atomic<typeT*> _instance;
  };

  template < typename typeT > std::mutex singleton<typeT>::_lock;
  template < typename typeT > std::atomic<typeT*> singleton<typeT>::_instance;
}

#endif
