#include <codex/vision/image_proc.hpp>

namespace codex { namespace vision {

  void histogram_equation( const image& src , image& dst )
  {
      int hist[256] = {0};
      int norm[256] = {0};

      for ( std::size_t y = 0 ; y < src.height() ; ++y ){
          for ( std::size_t x = 0 ; x < src.width() ; ++x ) {
              hist[ src.at(x,y)] += 1;
          }
      }
      double nop = src.width() * src.height();
      int sum = 0;
      for ( int i = 0 ; i < 256; ++i ) {
          sum += hist[i];
          norm[i] = sum / nop * 255;
      }
      for ( std::size_t y = 0 ; y < src.height() ; ++y ){
          for ( std::size_t x = 0 ; x < src.width() ; ++x ) {
              dst.at(x,y) = norm[src.at(x,y)];
          }
      }
  }

}}
