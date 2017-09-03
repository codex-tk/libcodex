#include <codex/vision/image_proc.hpp>
#include <codex/vision/image_draw.hpp>
#include <cmath>
namespace codex { namespace vision {

  void histogram_equation( const image& src , image& dst ,const std::size_t channel )
  {
      int hist[256] = {0};
      int norm[256] = {0};

      assert( channel < src.channel());
      assert( channel < dst.channel());

      for ( std::size_t y = 0 ; y < src.height() ; ++y ){
          const uint8_t* src_ptr = src.ptr(y);
          for ( std::size_t x = 0 ; x < src.width() ; ++x ) {
              hist[ src_ptr[ x * src.channel() + channel ] ] += 1;
          }
      }
      double nop = static_cast<double>(src.width() * src.height());
      int sum = 0;
      for ( int i = 0 ; i < 256; ++i ) {
          sum += hist[i];
          norm[i] = static_cast<int>( sum / nop * 255 );
      }
      for ( std::size_t y = 0 ; y < src.height() ; ++y ){
          const uint8_t* src_ptr = src.ptr(y);
          uint8_t* dst_ptr = dst.ptr(y);
          for ( std::size_t x = 0 ; x < src.width() ; ++x ) {
              dst_ptr[x * dst.channel() + channel] = norm[src_ptr[ x * src.channel() + channel ]];
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
      double nop = static_cast<double>(src.width() * src.height());
      int sum = 0;
      for ( int i = 0 ; i < 256; ++i ) {
          sum += hist[i];
          norm[i] = static_cast<int>(sum / nop * 255);
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

  double sqrt(double v ){
      double n = 1;
      for ( int i = 0; i < 10 ; ++i ){
          n =  0.5 * ( n + (v / n) );
      }
      return n;
  }

  void fft1d( double* re , double* im , int N , int isign ){
      int j=0;
      for ( int i = 0 ; i < N - 1 ; ++i ) {
          if ( i < j ) {
              std::swap( re[i] , re[j]);
              std::swap( im[i] , im[j]);
          }
          int k = N >> 1;
          while ( k <= j ) {
              j -= k;
              k >>= 1;
          }
          j += k;
      }

      int mmax, m;
      double wtemp, wr, wpr, wpi, wi, theta;
      double tempr, tempi;

      mmax = 2;
      while ( mmax <= N ) {
          theta = isign*(6.28318530717959/mmax);
          wtemp = sin(0.5*theta);
          wpr = -2.0*wtemp*wtemp;
          wpi = sin(theta);
          wr = 1.0;
          wi = 0.0;
          int step = mmax / 2;
          for ( m = 0 ; m <step ; ++m ) {
              for ( int i = m ; i < N ; i += mmax ) {
                  j = i + step;
                  tempr = double (wr*re[j]-wi*im[j]);
                  tempi = double (wr*im[j]+wi*re[j]);
                  re[j] = re[i]-tempr;
                  im[j] = im[i]-tempi;
                  re[i] += tempr;
                  im[i] += tempi;
              }
              wr = (wtemp=wr)*wpr-wi*wpi+wr;
              wi = wi*wpr+wtemp*wpi+wi;
          }
          mmax <<= 1;
      }
      if(isign == -1)
      {
          for (int i=0 ; i< N; i++)
          {
              re[i] /= N;
              im[i] /= N;
          }
      }
  }

  int fft_size( int n ){
      // log a(base) b -> log b / log a
      // 2 ^ x = n  -> log 2(base) n
      double x = log(static_cast<double>(n)) / log(2.0);
      x += 0.9999;
      return pow( 2.0 , static_cast<int>( x ) );
  }

  void fft_mag_image( const image_base<double>& re , const image_base<double>& im , image_base<uint8_t>& dst ){
      image_base<double> temp( re.width() , re.height() );
      double max_mag = 0;
      for ( std::size_t r = 0 ; r < re.height() ; ++r ) {
          const double* re_ptr = re.ptr(r);
          const double* im_ptr = im.ptr(r);
          double* temp_ptr = temp.ptr(r);
          for ( std::size_t c = 0 ; c < re.width() ; ++c ) {
              temp_ptr[c] = log( sqrt( re_ptr[c] * re_ptr[c] + im_ptr[c] * im_ptr[c] ) + 1 );
              if ( temp_ptr[c] > max_mag ) {
                  max_mag = temp_ptr[c];
              }
          }
      }
      for ( std::size_t r = 0 ; r < re.height() ; ++r ) {
          double* temp_ptr = temp.ptr(r);
          uint8_t* dst_ptr = dst.ptr(r);
          for ( std::size_t c = 0 ; c < re.width() ; ++c ) {
              dst_ptr[c] = static_cast<uint8_t>( temp_ptr[c] * ( 255 / max_mag) );
          }
      }
  }


  void fft_phs_image( const image_base<double>& re , const image_base<double>& im , image_base<uint8_t>& dst ){
      image_base<double> temp( re.width() , re.height() );
      for ( std::size_t r = 0 ; r < re.height() ; ++r ) {
          const double* re_ptr = re.ptr(r);
          const double* im_ptr = im.ptr(r);
          double* temp_ptr = temp.ptr(r);
          for ( std::size_t c = 0 ; c < re.width() ; ++c ) {
              temp_ptr[c] = atan2( im_ptr[c] , re_ptr[c] );
          }
      }
      for ( std::size_t r = 0 ; r < re.height() ; ++r ) {
          double* temp_ptr = temp.ptr(r);
          uint8_t* dst_ptr = dst.ptr(r);
          for ( std::size_t c = 0 ; c < re.width() ; ++c ) {
              dst_ptr[c] = static_cast<uint8_t>( (temp_ptr[c] * ( 127 / M_PI )) + 128 );
          }
      }
  }

  void fft_shift( image_base<double>& img ) {
       std::vector<double> swap_buffer( img.stride() );
       std::size_t rhalf = img.height() / 2;
       std::size_t chalf = img.width() / 2;
       for ( std::size_t r = 0 ; r < rhalf ; ++r ) {
           double* lt = img.ptr(r);
           double* lm = img.ptr(r + rhalf);
           // keep lt
           memcpy( &swap_buffer[0] , lt , img.width() * sizeof(double) );

           memcpy( lt , lm + chalf , chalf * sizeof(double) );               // 4 -> 1
           memcpy( lt + chalf , lm , chalf * sizeof(double));                // 3 -> 2
           memcpy( lm , &swap_buffer[0] + chalf , chalf * sizeof(double));  // 2 -> 3
           memcpy( lm + chalf, &swap_buffer[0] , chalf * sizeof(double) );   // 1 -> 4
       }
  }

  /*
      namespace detail{

          template < typename srcT ,
                     typename dstT ,
                     template < size_t R , size_t C , typename typeT > kernelT
                     >
          void convolution( const image_base<srcT>& src
                      , const kernelT& kernel
                      , image_base<dstT>& out )
          {
              typedef std::conditional<
                      std::is_floating_point< srcT >::value ||
                      std::is_floating_point< kernelT >::value , double , int64_t >::type value_type;
              std::size_t channel = src.channel();

              std::array<const srcT* , R > row_ptrs;

              int r_step = R/2;
              int c_step = C/2;

              for ( std::size_t r = r_step ; r < src.height() - r_step ; ++r ) {
                  dstT* out_ptr = out.ptr(r);
                  for ( int i = -r_step ; i <= r_step ; ++i ){
                      row_ptrs[i + r_step] = src.ptr( r + i );
                  }
                  for ( std::size_t c = c_step ; c < src.width() - c_step ; ++c ) {
                      std::size_t c_idx = c * channel;
                      for ( std::size_t ch = 0 ; ch < channel ; ++ ch ) {
                          c_idx += ch;
                          value_type val = 0;
                          for ( std::size_t i = 0 ; i < R * C ; ++i ) {
                              val += (row_ptrs[ i / R ][ c + ( i % C ) - c_step ] * kernel[i]);
                          }
                          out_ptr[c_idx] = codex::vision::operation< dstT ,value_type >::clip(val);
                      }
                  }
              }
          }



          template < typename typeT >
          void convolution( const image_base<typeT>& src
                      , const kernel<3,3,double>& kernel
                      , image_base<double>& out
                      , std::array< normalize_info , 4 >& norm )
          {
              std::size_t channel = src.channel();
              for ( std::size_t r = 1 ; r < src.height() - 1 ; ++r ) {
                   double* out_ptr = out.ptr(r);
                  const typeT* src_ptr_prev = src.ptr(r-1);
                  const typeT* src_ptr = src.ptr(r);
                  const typeT* src_ptr_next = src.ptr(r+1);
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

                          out_ptr[c_idx] = val;
                          if ( val < norm[ch].min ){
                              norm[ch].min = val;
                          }
                          if ( val > norm[ch].max ){
                              norm[ch].max = val;
                          }
                      }
                  }
              }
          }

          template < typename typeT , size_t R , size_t C , typename kernel_data_typeT >
          void convolution( const image_base<typeT>& src
                      , const kernel<R,C,kernel_data_typeT>& kernel
                      , image_base<double>& out )
          {
              std::array< detail::normalize_info , 4 > norm;
              for ( std::size_t i = 0 ; i < 4 ; ++i ) {
                  norm[i].min = std::numeric_limits<double>::max();
                  norm[i].max = std::numeric_limits<double>::min();
                  norm[i].factor = 0;
              }
              convolution( src ,kernel , out , norm);
          }
          template < typename typeT >
          void normalize(  const image_base<double>& src
                     , std::array< normalize_info , 4 >& norm
                     , image_base<typeT>& dst )
          {
              std::size_t channel = src.channel();
              for ( std::size_t i = 0 ; i < channel ; ++i ) {
                  norm[i].factor = std::numeric_limits<typeT>::max()
                          / fabs( norm[i].max - norm[i].min  );
              }
              for ( std::size_t r = 1 ; r < src.height() - 1 ; ++r ) {
                  const double* tmp_ptr = src.ptr(r);
                  typeT* dst_ptr = dst.ptr(r);
                  for ( std::size_t c = 1 ; c < src.width() - 1 ; ++c ) {
                      std::size_t c_offset = c * channel;
                      for ( std::size_t ch = 0 ; ch < channel ; ++ ch ) {
                          std::size_t c_idx = c_offset + ch;
                          if ( norm[ch].min < 0 ) {
                              dst_ptr[c_idx] = codex::vision::operation<typeT,double>::clip(
                                          (tmp_ptr[c_idx] + (norm[ch].min * -1) )* norm[ch].factor );
                          } else {
                              dst_ptr[c_idx] = codex::vision::operation<typeT,double>::clip(
                                          (tmp_ptr[c_idx] - norm[ch].min )* norm[ch].factor );
                          }
                      }
                  }
              }
          }
      }
  */
/*
  template <>
  void convolution<uint8_t,3,3,double>( const image_base<uint8_t>& src
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
  //}

/*
    template < typename typeT , size_t R , size_t C , typename kernel_data_typeT >
    void conv( const image_base<typeT>& src
                          , const kernel<R,C,kernel_data_typeT>& kernel
                          , image_base<typeT>& dst )
    {
        std::size_t channel = src.channel();

        std::array<const typeT* , R > row_ptrs;
        int r_step = R/2;
        int c_step = C/2;

        if ( channel == 1 ){
            for ( std::size_t r = r_step ; r < src.height() - r_step ; ++r ) {
                typeT* dst_ptr = dst.ptr(r);
                for ( int i = -r_step ; i <= r_step ; ++i ){
                    row_ptrs[i + r_step] = src.ptr( r + i );
                }
                for ( std::size_t c = c_step ; c < src.width() - c_step ; ++c ) {
                    double val = 0;
                    for ( std::size_t i = 0 ; i < R * C ; ++i ) {
                        val += (row_ptrs[ i / R ][ c + ( i % C ) - c_step ] * kernel[i]);
                    }
                    if ( val <= 0 ) val += 128;
                    dst_ptr[c] = codex::vision::operation<typeT>::convert(val);
                }
            }
        } else {
            for ( std::size_t r = r_step ; r < src.height() - r_step ; ++r ) {
                typeT* dst_ptr = dst.ptr(r);
                for ( int i = -r_step ; i <= r_step ; ++i ){
                    row_ptrs[i + r_step] = src.ptr( r + i );
                }
                for ( std::size_t c = c_step ; c < src.width() - c_step ; ++c ) {
                    std::size_t c_idx = c * channel;
                    for ( std::size_t ch = 0 ; ch < channel ; ++ ch ) {
                        c_idx += ch;
                        double val = 0;
                        for ( std::size_t i = 0 ; i < R * C ; ++i ) {
                            val += (row_ptrs[ i / R ][ c + ( i % C ) - c_step ] * kernel[i]);
                        }
                        if ( val <= 0 ) val += 128;
                        dst_ptr[c_idx] = codex::vision::operation<typeT>::convert(val);
                    }
                }
            }
        }
    }

    template <>
    void conv<uint8_t,3,3,double>( const image_base<uint8_t>& src
                          , const kernel<3,3,double>& kernel
                          , image_base<uint8_t>& dst );


    template < typename typeT >
    void sobel( const image_base<typeT>& src
                , image_base<typeT>& dst )
    {

    }
*//*
    template < typename typeT , typename kernelT >
    void convolution( const image_base<typeT>& src
                          , const kernelT& kernel
                          , image_base<typeT>& dst )
    {
        assert( is_same_format(src,dst));
        std::size_t channel = src.channel();
        int row_step = kernel.rows() / 2;
        int col_step = kernel.cols() / 2;
        //double div = kernel.rows() * kernel.cols();
        if ( channel == 1 ) {
            for ( std::size_t r = 0 ; r < src.height() ; ++r ) {
                typeT* dst_ptr = dst.ptr(r);
                for ( std::size_t c = 0 ; c < src.width() ; ++c ) {
                    double sum = 0;
                    for ( int y = 0 ; y < kernel.rows() ; ++y ) {
                        int row_index = r + y - row_step;
                        if ( row_index >= 0 && row_index < src.height() ) {
                            const typeT* src_ptr = src.ptr(row_index);
                            for ( int x = 0 ; x < kernel.cols() ; ++x ) {
                                int col_index = c + x - col_step;
                                if ( col_index >= 0 && col_index < src.width()){
                                    sum += src_ptr[ col_index ] * kernel.at(x,y);
                                }
                            }
                        }
                    }
                    *dst_ptr = codex::vision::operation<typeT>::convert( sum );
                    ++dst_ptr;
                }
            }
        } else {
            for ( std::size_t r = 0 ; r < src.height() ; ++r ) {
                typeT* dst_ptr = dst.ptr(r);
                for ( std::size_t c = 0 ; c < src.width() ; ++c ) {
                    for ( std::size_t ch = 0 ; ch < channel ; ++ch ) {
                        double sum = 0;
                        for ( int y = 0 ; y < kernel.rows() ; ++y ) {
                            int row_index = r + y - row_step;
                            if ( row_index >= 0 && row_index < src.height() ) {
                                const typeT* src_ptr = src.ptr(row_index);
                                for ( int x = 0 ; x < kernel.cols() ; ++x ) {
                                    int col_index = c * channel + x - col_step;
                                    if ( col_index >= 0 && col_index < src.width()){
                                        sum += src_ptr[ col_index ] * kernel.at(x,y);
                                    }
                                }
                            }
                        }
                        *dst_ptr = codex::vision::operation<typeT>::convert( sum );
                        ++dst_ptr;
                    }
                }
            }
        }

    }
 * */
}}
