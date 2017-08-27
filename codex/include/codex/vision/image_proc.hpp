#ifndef __codex_vision_image_proc_h__
#define __codex_vision_image_proc_h__

#include <cassert>
#include <array>
#include <codex/vision/image.hpp>
#include <cmath>

namespace codex { namespace vision {

    template < size_t R , size_t C , typename typeT = double >
    class kernel {
    public:
        kernel(){}

        template < typename ...argsT >
        kernel( argsT... args )
            : _buffer{{ args ... }}{
        }

        typeT& at( std::size_t x , std::size_t y ) {
            return _buffer[y * C + x];
        }

        const typeT& at( std::size_t x , std::size_t y ) const {
            return _buffer[y * C + x];
        }

        std::size_t rows() const{
            return R;
        }

        std::size_t cols() const{
            return R;
        }

        typeT operator[] ( int idx ) const {
            return _buffer[idx];
        }

    private:
        std::array< typeT , R * C > _buffer;
    };

    static codex::vision::kernel<3,3> laplacian {
        0.0 ,  1.0 ,  0.0 ,
        1.0 , -4.0 ,  1.0 ,
        0.0 ,  1.0 ,  0.0
    };

    static codex::vision::kernel<3,3> edge {
        -1.0 , -1.0 , -1.0 ,
        -1.0 ,  9.0 , -1.0,
        -1.0 , -1.0 , -1.0
    };
    static codex::vision::kernel<3,3> sobel_x {
       -1.0 ,  0.0 ,  1.0 ,
       -2.0 ,  0.0 ,  2.0 ,
       -1.0 ,  0.0 ,  1.0
    };
    static codex::vision::kernel<3,3> sobel_y {
         1.0 ,  2.0 ,  1.0 ,
         0.0 ,  0.0 ,  0.0 ,
        -1.0 , -2.0 , -1.0
    };

    namespace detail{

        struct normalize_info {
            double min;
            double max;
            double factor;
        };

        template < typename typeT , size_t R , size_t C , typename kernel_data_typeT >
        void convolution( const image_base<typeT>& src
                    , const kernel<R,C,kernel_data_typeT>& kernel
                    , image_base<double>& out
                    , std::array< normalize_info , 4 >& norm )
        {
            std::size_t channel = src.channel();

            std::array<const typeT* , R > row_ptrs;
            int r_step = R/2;
            int c_step = C/2;

            for ( std::size_t r = r_step ; r < src.height() - r_step ; ++r ) {
                double* out_ptr = out.ptr(r);
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
                            dst_ptr[c_idx] = codex::vision::operation<uint8_t>::convert(
                                        (tmp_ptr[c_idx] + (norm[ch].min * -1) )* norm[ch].factor );
                        } else {
                            dst_ptr[c_idx] = codex::vision::operation<uint8_t>::convert(
                                        (tmp_ptr[c_idx] - norm[ch].min )* norm[ch].factor );
                        }
                    }
                }
            }
        }
    }

    template < typename typeT >
    image gray_scale( const image_base<typeT>& src  ){
        image dst( src.width() , src.height());
        for ( std::size_t y = 0 ; y < src.height() ; ++y ){
            uint8_t* dst_ptr = dst.ptr(y);
            for ( std::size_t x = 0 ; x < src.width() ; ++x ) {
                int val = 0;
                for ( std::size_t c = 0 ; c < src.channel() ; ++c ) {
                    val += src.at(x,y,c);
                }
                *dst_ptr = codex::vision::operation<uint8_t>::convert(val/src.channel());
                ++dst_ptr;
            }
        }
        return dst;
    }

    void histogram_equation( const image& src , image& dst );
    void histogram_equation_debug( const image& src
                                   , image& dst
                                   , image& orig_hist
                                   , image& equation_hist );
    void histogram_graph( const image& src , image& dst );



    template < typename typeT , size_t R , size_t C , typename kernel_data_typeT >
    void convolution( const image_base<typeT>& src
                          , const kernel<R,C,kernel_data_typeT>& kernel
                          , image_base<typeT>& dst )
    {
        assert( is_same_format(src,dst));
        image_base<double> temp(src.width(),src.height(),src.channel());
        std::array< detail::normalize_info , 4 > norm;
        for ( std::size_t i = 0 ; i < 4 ; ++i ) {
            norm[i].min = std::numeric_limits<double>::max();
            norm[i].max = std::numeric_limits<double>::min();
            norm[i].factor = 0;
        }
        detail::convolution( src , kernel , temp , norm );
        detail::normalize( temp , norm , dst );
    }

    template < typename typeT >
    void sobel( const image_base<typeT>& src , image_base<typeT>& dst ){
        assert( is_same_format(src,dst));

        image_base<double> sobx(src.width(),src.height(),src.channel());
        image_base<double> soby(src.width(),src.height(),src.channel());
        std::array< detail::normalize_info , 4 > norm;
        for ( std::size_t i = 0 ; i < 4 ; ++i ) {
            norm[i].min = std::numeric_limits<double>::max();
            norm[i].max = std::numeric_limits<double>::min();
            norm[i].factor = 0;
        }
        detail::convolution( src , sobel_x , sobx , norm );
        for ( std::size_t i = 0 ; i < 4 ; ++i ) {
            norm[i].min = std::numeric_limits<double>::max();
            norm[i].max = std::numeric_limits<double>::min();
            norm[i].factor = 0;
        }
        detail::convolution( sobx, sobel_y , soby , norm );
        detail::normalize( soby , norm , dst );
    }

    double sqrt( double v );
}}


#endif
