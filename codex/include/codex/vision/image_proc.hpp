#ifndef __codex_vision_image_proc_h__
#define __codex_vision_image_proc_h__

#include <cassert>
#include <array>
#include <codex/vision/image.hpp>

namespace codex { namespace vision {

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

    static codex::vision::kernel<3,3> laplacian_mask{
        0.0 , -1.0 , 0.0
        , -1.0, 4.0, -1.0
        ,0.0 , -1.0 , 0.0
    };

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
}}


#endif
