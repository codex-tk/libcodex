#include <codex/vision/fft.hpp>
#include <cmath>

namespace codex { namespace vision {


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
        theta = isign * ( 6.28318530717959 / mmax );
        wtemp = sin( 0.5 * theta );
        wpr = -2.0 * wtemp * wtemp;
        wpi = sin(theta);
        wr = 1.0;
        wi = 0.0;
        int step = mmax / 2;
        for ( m = 0 ; m <step ; ++m ) {
            for ( int i = m ; i < N ; i += mmax ) {
                j = i + step;
                tempr = static_cast<double>( wr * re[j] - wi * im[j] );
                tempi = static_cast<double>( wr * im[j] + wi * re[j] );
                re[j] = re[i]-tempr;
                im[j] = im[i]-tempi;
                re[i] += tempr;
                im[i] += tempi;
            }
            wtemp = wr;
            wr = wtemp * wpr - wi * wpi + wr;
            wi = wi * wpr + wtemp * wpi + wi;
        }
        mmax <<= 1;
    }
    if(isign == -1){
        for (int i=0 ; i< N; i++){
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

void fft( const image_base<uint8_t>& gray_image
  , image_base<double>& fft_re
  , image_base<double>& fft_im )
{
  fft_re = codex::vision::image_base<double>(
              codex::vision::fft_size( gray_image.width() )
              , codex::vision::fft_size( gray_image.height())
              , 1 );
  fft_im = codex::vision::image_base<double>(
              codex::vision::fft_size( gray_image.width() )
              , codex::vision::fft_size( gray_image.height())
              , 1 );
  fft_re.reset(0);
  fft_im.reset(0);
  fft_re.put_channnel( 0 , gray_image , 0 );
  for ( std::size_t r = 0 ; r < fft_re.height() ; ++r ) {
      codex::vision::fft1d( fft_re.ptr(r) , fft_im.ptr(r) , fft_re.width() , 1 );
  }
  fft_re = fft_re.transpose();
  fft_im = fft_im.transpose();
  for ( std::size_t r = 0 ; r < fft_re.height() ; ++r ) {
      codex::vision::fft1d( fft_re.ptr(r) , fft_im.ptr(r) , fft_re.width() , 1 );
  }
  fft_re = fft_re.transpose();
  fft_im = fft_im.transpose();
}

}}
