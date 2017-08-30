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
        typedef typeT value_type;
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
    static codex::vision::kernel<3, 3> laplacian8{
      1.0 ,  1.0 ,  1.0 ,
      1.0 , -8.0 ,  1.0 ,
      1.0 ,  1.0 ,  1.0
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

    namespace  detail {
        template < typename srcT
                   , typename dstT
                   , size_t R
                   , size_t C
                   , typename kernelValueT
                   , typename handlerT
                   >
        void filter( const image_base< srcT >& src
                     , image_base< dstT >& dst
                     , const kernel< R , C , kernelValueT >& kernel
                     , const handlerT& handler )
        {
            assert( src.width() == dst.width());
            assert( src.height() == dst.height());
            assert( src.channel() == dst.channel());

            typedef typename std::conditional<
                        std::is_floating_point< srcT >::value
                        || std::is_floating_point< kernelValueT >::value
                    , double , int >::type value_type;

            std::array< const srcT* , R > row_ptrs;

            int r_step = R / 2;
            int c_step = C / 2;

            for ( int r = 0 ; r < static_cast<int>(src.height()) ; ++r ) {
                dstT* dst_ptr = dst.ptr(r);
                for ( int i = -r_step ; i <= r_step ; ++i ) {
                    int row_idx = r + i;
                    if ( row_idx >= 0 && row_idx < static_cast<int>(src.height()) ){
                        row_ptrs[i+r_step] = src.ptr( row_idx );
                    } else {
                        row_ptrs[i+r_step] = nullptr;
                    }
                }

                for ( int c = 0 ; c < static_cast<int>(src.width()) ; ++c ) {
                    for ( int l = 0 ; l < static_cast<int>(src.channel()) ; ++l ) {
                        value_type val = 0;
                        int k_idx = 0;
                        for ( int i = -r_step ; i <= r_step ; ++i ) {
                            const srcT* src_ptr = row_ptrs[i + r_step];
                            if ( src_ptr ) {
                                for ( int j = -c_step ; j <= c_step ; ++j ) {
                                    int col_idx = c + j;
                                    if ((col_idx >= 0) && (col_idx < static_cast<int>(src.width())))
                                    {
                                        kernelValueT kv = kernel[k_idx];
                                        if ( kv != 0 ) {
                                            val += (src_ptr[ col_idx * src.channel() + l] * kv);
                                        }
                                    }
                                    ++k_idx;
                                }
                            }
                        }
                        dst_ptr[ c * dst.channel() + l ] = handler(val);
                    }
                }
            }
        }

        template < typename srcT , typename dstT , typename handlerT >
        void filter( const image_base<srcT>& src
                    , image_base<dstT>& out
                    , const kernel<3,3,double>& kernel
                    , const handlerT& handler  )
        {
            std::size_t channel = src.channel();
            for ( std::size_t r = 1 ; r < src.height() - 1 ; ++r ) {
                dstT* out_ptr = out.ptr(r);
                const srcT* src_ptr_prev = src.ptr(r-1);
                const srcT* src_ptr = src.ptr(r);
                const srcT* src_ptr_next = src.ptr(r+1);
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

                        out_ptr[c_idx] = handler(val);
                    }
                }
            }
        }

        template < typename srcT , typename dstT >
        void normalize( const image_base<srcT>& src
                   , image_base<dstT>& dst )
        {
            double factor[4];
            srcT min_val[4];
            srcT max_val[4];

            for ( int i = 0 ; i < 4; ++i ) {
                factor[i] = 0;
                min_val[i] = std::numeric_limits<srcT>::max();
                max_val[i] = std::numeric_limits<srcT>::min();
            }


            for ( std::size_t r = 0; r < src.height() ; ++r ) {
                const srcT* src_ptr = src.ptr(r);
                for ( std::size_t c = 0; c < src.height() ; ++c ) {
                    for ( std::size_t l = 0 ; l < src.channel() ; ++l ) {
                        srcT val = src_ptr[ c * src.channel() + l];
                        if ( min_val[l] > val ) {
                            min_val[l] = val;
                        }
                        if ( max_val[l] < val ) {
                            max_val[l] = val;
                        }
                    }
                }
            }

            std::size_t channel = std::min( src.channel() ,dst.channel());

            for ( std::size_t i = 0 ; i < channel ; ++i ) {
                factor[i] = std::numeric_limits<dstT>::max()
                        / std::abs( max_val[i] - min_val[i] );
            }
            for ( std::size_t r = 1 ; r < src.height() - 1 ; ++r ) {
                const srcT* tmp_ptr = src.ptr(r);
                dstT* dst_ptr = dst.ptr(r);
                for ( std::size_t c = 1 ; c < src.width() - 1 ; ++c ) {
                    std::size_t src_c_offset = c * src.channel();
                    std::size_t dst_c_offset = c * dst.channel();
                    for ( std::size_t l = 0 ; l < channel ; ++l ) {
                        std::size_t src_c_idx = src_c_offset + l;
                        std::size_t dst_c_idx = dst_c_offset + l;
                        if ( min_val[l] < 0 ) {
                            dst_ptr[dst_c_idx] = codex::vision::operation<dstT,srcT>::clip(
                                        (tmp_ptr[src_c_idx] + (min_val[l] * -1) )* factor[l] );
                        } else {
                            dst_ptr[dst_c_idx] = codex::vision::operation<dstT,srcT>::clip(
                                        (tmp_ptr[src_c_idx] - min_val[l] )* factor[l] );
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
            const typeT* src_ptr = src.ptr(y);
            for ( std::size_t x = 0 ; x < src.width() ; ++x ) {
                double val = 0;
                for ( std::size_t c = 0 ; c < src.channel() ; ++c ) {
                    val += src_ptr[ x * src.channel() + c ];
                }
                *dst_ptr = codex::vision::operation<uint8_t,typeT>::clip(val/src.channel());
                ++dst_ptr;
            }
        }
        return dst;
    }

    void histogram_equation( const image& src , image& dst , const std::size_t channel = 0);
    void histogram_equation_debug( const image& src
                                   , image& dst
                                   , image& orig_hist
                                   , image& equation_hist );
    void histogram_graph( const image& src , image& dst );

    template < typename typeT >
    void gaussian( const image_base<typeT>& src , image_base<typeT>& dst
                   , int k_size = 3
                   , double standard_deviation = 0.5 )
    {
        assert( is_same_format(src,dst));

        double gauss[32] = {0};
        if ( k_size % 2 == 0 ) {
            k_size += 1;
        }
        if ( k_size < 3 ) k_size = 3;
        if ( k_size > 9 ) k_size = 9;

        int half = k_size / 2;
        for ( int i = 0; i <= half ; ++i ) {
            gauss[ half - i ] = gauss[ half + i] = exp( ( i * i ) /(  -2 * standard_deviation * standard_deviation));
        }

        int width = src.width();
        int height = src.height();
        int channel = src.channel();

        image_base<typeT> intermediate( width , height, channel );

        for ( int r = 0 ; r < height; ++r ){
            const typeT* ptr = src.ptr(r);
            typeT* im_ptr = intermediate.ptr(r);
            for ( int c = 0 ; c < width ; ++c ) {
                for ( int l = 0 ; l < channel ; ++l ) {
                    double sum = 0;
                    double tot = 0;
                    for ( int px = -half ; px <= half ; ++px ) {
                        int col_index = c + px;
                        if ( col_index >= 0 && col_index < width ) {
                            sum += ptr[col_index * channel + l ] * gauss[ half + px ];
                            tot += gauss[half+px];
                        }
                    }
                    im_ptr[ c * intermediate.channel() + l ] = static_cast<typeT>( sum/tot );
                }
            }
        }

        for ( int r = 0 ; r < height; ++r ){
            typeT* dst_ptr = dst.ptr(r);
            for ( int c = 0 ; c < width ; ++c ) {
                for ( int l = 0 ; l < channel ; ++l ) {
                    double sum = 0;
                    double tot = 0;
                    for ( int py = -half ; py <= half ; ++py ) {
                        int row_index = r + py;
                        if ( row_index >= 0 && row_index < height ) {
                            typeT* im_ptr = intermediate.ptr(row_index);
                            sum += im_ptr[c * channel + l ] * gauss[ half + py ];
                            tot += gauss[half+py];
                        }
                    }
                    dst_ptr[ c * channel + l ] = static_cast<typeT>( sum/tot );
                }
            }
        }
    }

    template < typename typeT >
    void mean( const image_base<typeT>& src , image_base<typeT>& dst
                   , int k_size = 3 )
    {
        assert( is_same_format(src,dst));
        if ( k_size % 2 == 0 ) {
            k_size += 1;
        }
        if ( k_size < 3 ) k_size = 3;
        if ( k_size > 9 ) k_size = 9;

        int half = k_size / 2;

        int width = src.width();
        int height = src.height();
        int channel = src.channel();

        typedef decltype ( std::numeric_limits<typeT>::max() + 1 ) value_type;

        for ( int r = 0 ; r < height ; ++r ) {
            typeT* dst_ptr = dst.ptr(r);
            for ( int c = 0 ; c < width ; ++c ) {
            for ( int l = 0 ; l < channel ; ++l ) {
                value_type sum = 0;
                double count = 0;
                for ( int py = -half ; py <= half ; ++py ) {
                    int row_idx = r + py;
                    if ( row_idx >= 0 && row_idx < height ) {
                        const typeT* src_ptr = src.ptr(row_idx);
                        for ( int px = -half ; px <= half ; ++px ) {
                            int col_idx = c + px;
                            if ( col_idx >= 0 && col_idx < width ) {
                                sum += src_ptr[ col_idx * channel + l ];
                                count += 1;
                            }
                        }
                    }
                }
                dst_ptr[ c * channel + l ] = operation< typeT , double >::clip( sum / count );

            }   // channel
            }   // column
        }   // row
    }

    template < typename typeT >
    void median( const image_base<typeT>& src , image_base<typeT>& dst
                   , int k_size = 3 )
    {
        assert( is_same_format(src,dst));
        if ( k_size % 2 == 0 ) {
            k_size += 1;
        }
        if ( k_size < 3 ) k_size = 3;
        if ( k_size > 9 ) k_size = 9;

        int half = k_size / 2;

        int width = src.width();
        int height = src.height();
        int channel = src.channel();

        std::array< typeT , 32 > values;

        for ( int r = 0 ; r < height ; ++r ) {
            typeT* dst_ptr = dst.ptr(r);
            for ( int c = 0 ; c < width ; ++c ) {
            for ( int l = 0 ; l < channel ; ++l ) {
                int index = 0;
                for ( int py = -half ; py <= half ; ++py ) {
                    int row_idx = r + py;
                    if ( row_idx >= 0 && row_idx < height ) {
                        const typeT* src_ptr = src.ptr(row_idx);
                        for ( int px = -half ; px <= half ; ++px ) {
                            int col_idx = c + px;
                            if ( col_idx >= 0 && col_idx < width ) {
                                values[index++] = src_ptr[ col_idx * channel + l ];
                            }
                        }
                    }
                }
                std::sort( &values[0] ,  &values[0] + index  );
                dst_ptr[ c * channel + l ] = values[ index / 2 ];

            }   // channel
            }   // column
        }   // row
    }

    double sqrt( double v );
}}


#endif
