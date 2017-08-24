#include <codex/vision/image_proc.hpp>
#include <codex/vision/image_draw.hpp>
#include <cmath>
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

  template <>
  void conv<uint8_t,3,3,double>( const image_base<uint8_t>& src
                        , const kernel<3,3,double>& kernel
                        , image_base<uint8_t>& dst )
  {
      std::size_t channel = src.channel();
      image_base<double> temp(src.width(),src.height(),channel);
      std::array< std::pair<double,double> , 4 > ranges;
      std::array< double , 4 > factor;
      for ( std::size_t i = 0 ; i < 4 ; ++i ) {
          ranges[i] = std::make_pair( std::numeric_limits<double>::max()
                                       , std::numeric_limits<double>::min());
          factor[i] = 0;
      }

      for ( std::size_t r = 1 ; r < src.height() - 1 ; ++r ) {
          double* tmp_ptr = temp.ptr(r);
          const uint8_t* src_ptr_prev = src.ptr(r-1);
          const uint8_t* src_ptr = src.ptr(r);
          const uint8_t* src_ptr_next = src.ptr(r+1);
          for ( std::size_t c = 1 ; c < src.width() - 1 ; ++c ) {
              std::size_t c_offset = c * channel;
              for ( std::size_t ch = 0 ; ch < channel ; ++ ch ) {
                  std::size_t c_idx = c_offset + ch;
                  tmp_ptr[c_idx]= (  src_ptr_prev[c_idx -1]  * kernel[0] +
                                  src_ptr_prev[c_idx]     * kernel[1] +
                                  src_ptr_prev[c_idx+1]   * kernel[2] +
                                  src_ptr[c_idx-1]        * kernel[3] +
                                  src_ptr[c_idx]          * kernel[4] +
                                  src_ptr[c_idx+1]        * kernel[5] +
                                  src_ptr_next[c_idx-1]   * kernel[6] +
                                  src_ptr_next[c_idx]     * kernel[7] +
                                  src_ptr_next[c_idx+1]   * kernel[8]);

                  if ( tmp_ptr[c_idx] < ranges[ch].first ){
                      ranges[ch].first = tmp_ptr[c_idx];
                  }
                  if ( tmp_ptr[c_idx] > ranges[ch].second ){
                      ranges[ch].second = tmp_ptr[c_idx];
                  }
              }
          }
      }
      for ( std::size_t i = 0 ; i < channel ; ++i ) {
          factor[i] = 255.0 / fabs( ranges[i].second - ranges[i].first );
      }

      for ( std::size_t r = 1 ; r < src.height() - 1 ; ++r ) {
          double* tmp_ptr = temp.ptr(r);
          uint8_t* dst_ptr = dst.ptr(r);
          for ( std::size_t c = 1 ; c < src.width() - 1 ; ++c ) {
              std::size_t c_offset = c * channel;
              for ( std::size_t ch = 0 ; ch < channel ; ++ ch ) {
                  std::size_t c_idx = c_offset + ch;
                  if ( ranges[ch].first < 0 ) {
                      dst_ptr[c_idx] = codex::vision::operation<uint8_t>::convert(
                                  (tmp_ptr[c_idx] + (ranges[ch].first * -1) )*factor[ch] );
                  } else {
                      dst_ptr[c_idx] = codex::vision::operation<uint8_t>::convert(
                                  (tmp_ptr[c_idx] - ranges[ch].first )*factor[ch] );
                  }
              }
          }
      }
/*
      std::size_t channel = src.channel();
      for ( std::size_t r = 1 ; r < src.height() - 1 ; ++r ) {
          uint8_t* dst_ptr = dst.ptr(r);
          const uint8_t* src_ptr_prev = src.ptr(r-1);
          const uint8_t* src_ptr = src.ptr(r);
          const uint8_t* src_ptr_next = src.ptr(r+1);
          for ( std::size_t c = 1 ; c < src.width() - 1 ; ++c ) {
              std::size_t c_offset = c * channel;
              for ( std::size_t ch = 0 ; ch < channel ; ++ ch ) {
                  std::size_t c_idx = c_offset + ch;
                  double val = (  src_ptr_prev[c_idx -1]  * kernel[0] +
                                  src_ptr_prev[c_idx]     * kernel[1] +
                                  src_ptr_prev[c_idx+1]   * kernel[2] +
                                  src_ptr[c_idx-1]        * kernel[3] +
                                  src_ptr[c_idx]          * kernel[4] +
                                  src_ptr[c_idx+1]        * kernel[5] +
                                  src_ptr_next[c_idx-1]   * kernel[6] +
                                  src_ptr_next[c_idx]     * kernel[7] +
                                  src_ptr_next[c_idx+1]   * kernel[8]);

                  if ( val <= 0 ) val += 128;
                  dst_ptr[c_idx] = codex::vision::operation<uint8_t>::convert(val);
              }
          }
      }*/
  }


}}
