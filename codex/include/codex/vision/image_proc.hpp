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
            for ( std::size_t x = 0 ; x < src.width() ; ++x ) {
                int val = 0;
                for ( std::size_t c = 0 ; c < src.channel() ; ++c ) {
                    val += src.at(x,y,c);
                }
                dst.at(x,y) = (val/src.channel());
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

    private:
        std::array< typeT , R * C > _buffer;
    };

    template < typename typeT , typename kernelT >
    void convolution_sum( const image_base<typeT>& src
                          , const kernelT& kernel
                          , image_base<typeT>& dst )
    {
        assert( is_same_format(src,dst));
        std::size_t channel = src.channel();
        int row_step = kernel.rows() / 2;
        int col_step = kernel.cols() / 2;
        double div = kernel.rows() * kernel.cols();
        for ( std::size_t r = 0 ; r < src.height() ; ++r ) {
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
                                /*
                                sum += (src.at_r( c + x - col_step
                                                 ,row_index
                                                 ,ch , 0) * kernel.at(x,y));*/
                            }
                        }

                    }
                    dst.at(c,r,ch) = codex::vision::operation<typeT>::convert( sum + 100);/// div);
                }
            }
        }
    }
}}


#endif
