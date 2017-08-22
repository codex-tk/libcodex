#include <codex/vision/image_proc.hpp>
#include <codex/vision/image_draw.hpp>

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

  void histogram_equation_debug( const image& src
                                 , image& dst
                                 , image& orig_hist
                                 , image& equation_hist )
  {
      orig_hist = image(256,256);
      equation_hist = image(256,256);

      int hist[256] = {0};
      int norm[256] = {0};
      double max_hist = 0;
      for ( std::size_t y = 0 ; y < src.height() ; ++y ){
          for ( std::size_t x = 0 ; x < src.width() ; ++x ) {
              hist[ src.at(x,y)] += 1;
              if ( hist[ src.at(x,y)] > max_hist ) {
                  max_hist = hist[ src.at(x,y)];
              }
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

      for ( int x = 0 ; x < 256; ++x ) {
          codex::vision::line_to( orig_hist
                                 , codex::vision::point{ x , 255 }
                                 , codex::vision::point{ x , static_cast<int>(
                                                         256 - (hist[x] / max_hist * 256)) }
                                 , 255 );
          codex::vision::line_to( equation_hist
                                 , codex::vision::point{ x , 255 }
                                 , codex::vision::point{ x , 256 - norm[x] }
                                 , 255 );
      }
  }

  void histogram_graph( const image& src , image& dst ) {
      dst = image(256,256);

      int hist[256] = {0};
      double max_hist = 0;
      for ( std::size_t y = 0 ; y < src.height() ; ++y ){
          for ( std::size_t x = 0 ; x < src.width() ; ++x ) {
              hist[ src.at(x,y)] += 1;
              if ( hist[ src.at(x,y)] > max_hist ) {
                  max_hist = hist[ src.at(x,y)];
              }
          }
      }
      for ( int x = 0 ; x < 256; ++x ) {
          codex::vision::line_to( dst
                                 , codex::vision::point{ x , 255 }
                                 , codex::vision::point{ x , static_cast<int>(
                                                         256 - (hist[x] / max_hist * 256)) }
                                 , 255 );
      }
  }

}}
