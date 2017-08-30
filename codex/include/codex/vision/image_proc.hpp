#ifndef __codex_vision_image_proc_h__
#define __codex_vision_image_proc_h__

#include <cassert>
#include <array>
#include <codex/vision/kernel.hpp>
#include <cmath>

namespace codex { namespace vision {

    template < typename typeT >
    image gray_scale( const image_base<typeT>& src  ){
        image dst( src.width() , src.height());
        for ( std::size_t y = 0 ; y < src.height() ; ++y ){
            uint8_t* dst_ptr = dst.ptr(y);
            const typeT* src_ptr = src.ptr(y);
            for ( std::size_t x = 0 ; x < src.width() ; ++x ) {
                if ( src.channel() == 3 || src.channel() == 4 ) {
                    // BGR to Y
                    *dst_ptr = 0.114 * src_ptr[ x * src.channel()]
                             + 0.587 * src_ptr[ x * src.channel() + 1]
                             + 0.299 * src_ptr[ x * src.channel() + 2];
                } else {
                    // mean
                    double val = 0;
                    for ( std::size_t c = 0 ; c < src.channel() ; ++c ) {
                        val += src_ptr[ x * src.channel() + c ];
                    }
                    *dst_ptr = codex::vision::operation<uint8_t,typeT>::clip(val/src.channel());
                }
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
