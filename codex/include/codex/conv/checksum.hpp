#ifndef __libcodex_checksum_h__
#define __libcodex_checksum_h__

namespace codex {

  template < typename typeT >
  class checksum
  {
  public:
    checksum() :_chksum(0) {}
    ~checksum(){}

    void update(const typeT& val) {
      _chksum += val;
    }

    typeT final(void) {
      typeT val = ~_chksum;
      val += 1;
      return val;
    }

  private:
    typeT _chksum;
  };
}

#endif