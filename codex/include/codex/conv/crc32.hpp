#ifndef __libcodex_convenience_crc32_h__
#define __libcodex_convenience_crc32_h__

#include <codex/codex.hpp> 

namespace codex {

  class crc32 {
  public:
    crc32(void);
    ~crc32(void) = default;

    uint32_t value(void) const;

    void put(const void* ptr, const int sz);
    void reset(void);
  private:
    uint32_t _crc;
  };

}

#endif