#ifndef __codex_vision_image_proc_h__
#define __codex_vision_image_proc_h__

#include <cassert>
#include <array>
#include <codex/vision/kernel.hpp>
#include <codex/vision/fft.hpp>
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

    void equalize_hist( const image& src , image& dst , const std::size_t channel = 0);
    void equalize_hist_debug( const image& src
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

      int clip( int orig , int max );

    template < typename typeT >
    typeT bilinear( const image_base<typeT>& src , double x , double y , std::size_t channel = 0 ){
        int xl = clip( static_cast<int>(x) , src.width() );
        int xr = clip( static_cast<int>(x) + 1 , src.width() );
        int yt = clip( static_cast<int>(y) , src.height() );
        int yb = clip( static_cast<int>(y) + 1 , src.height() );
        // 가중치 계산 대각선 방향에의 거리값( 크면 가깞고 , 작으면 멀다 )
        int lt = ( xr - x )*( yb - y );
        int rt = ( x - xl )*( yb - y );
        int lb = ( xr - x )*( y - yt );
        int rb = ( x - xl )*( y - yt );

        return codex::vision::operation<typeT,typeT>::clip(
                    lt * src.at(xl,yt,channel)
                    + rt * src.at(xr,yt,channel)
                    + lb * src.at(xl,yb,channel)
                    + rb * src.at(xr,yb,channel)
                    );
    }

    template < typename typeT >
    void tranform( const image_base<typeT>& src //
                   , image_base<typeT>& dst     //
                   , double rotation            // degree
                   , double scale               // zoom default 1
                   , double cx                  // center src.width() / 2.0
                   , double cy                  // center src.height() / 2.0
                   , double tx                  // trans x
                   , double ty )                // trans y
    {
        double cr = std::cos( rotation * M_PI/180.0 );
        double sr = std::sin( rotation * M_PI/180.0 );

        for ( std::size_t r = 0 ; r < src.height() ; ++r ) {
            for ( std::size_t c = 0 ; c < src.width() ; ++c ) {
                // 중심을 원점으로 이동
                // 회전 적용
                // 원점 복귀
                // 확대 / 축소
                // 평행 이동
                double srcx = scale * ( cr * (c - cx) + sr * ( r -cy )) + cx - tx;
                double srcy = scale * (-sr * (c - cx) + cr * ( r -cy )) + cy - ty;
                if ( srcx >= 0 && srcx < src.width() && srcy >= 0 && srcy < src.height() ) {
                    for ( std::size_t l = 0 ; l < src.channel() ; ++l ) {
                        dst.at(r,c,l) = bilinear( src , srcx , srcy , l );
                    }
                }
            }
        }
    }

    template < typename typeT >
    void binarization( const image_base<typeT>& src
                       , image_base<typeT>& dst )
    {
        int sum_hi[4] = {0};
        double cnt_hi[4] = {0};
        int sum_lo[4] = {0};
        double cnt_lo[4] = {0};
        typeT threashold[4] = {0};

        std::size_t channel = src.channel();
        typeT max_val = std::numeric_limits<typeT>::max();

        int cnt_change = 1;
        while ( cnt_change ){
            cnt_change = 0;
            for ( std::size_t i = 0 ; i < channel ; ++i ) {
                sum_lo[i] = sum_hi[i] = cnt_hi[i] = cnt_lo[i] = 0;
            }
            for ( std::size_t r = 0 ; r < src.height() ; ++r ) {
                const typeT* src_ptr = src.ptr(r);
                typeT* dst_ptr = dst.ptr(r);
                for ( std::size_t c = 0 ; c < src.width() ; ++c ) {
                    for ( std::size_t l = 0 ; l < channel ; ++l ) {
                        std::size_t index = c * channel + l;
                        const typeT val = src_ptr[index];
                        if ( val > threashold[l] ) {
                            if ( dst_ptr[index] != max_val)
                                ++cnt_change;
                            dst_ptr[index] = max_val;
                            sum_hi[l] += src_ptr[index];
                            cnt_hi[l] += 1;
                        } else {
                            if ( dst_ptr[index] != 0 )
                                ++cnt_change;
                            dst_ptr[index] = 0;
                            sum_lo[l] += src_ptr[index];
                            cnt_lo[l] += 1;
                        }
                    }
                }
            }
            for ( std::size_t i = 0 ; i < channel ; ++i ) {
                if ( sum_hi[i] == 0 ) {
                    threashold[i] = sum_lo[i] / cnt_lo[i];
                } else if ( sum_lo[i] == 0 ) {
                    threashold[i] = sum_hi[i] / cnt_hi[i];
                } else {
                    int hi = sum_hi[i] / cnt_hi[i];
                    int lo = sum_lo[i] / cnt_lo[i];
                    int avg = (hi + lo )/ 2;
                    threashold[i] = avg;
                }
            }
        }
    }

    template < typename typeT >
    void kmeans( const image_base<typeT>& src , image_base<typeT>& dst
               , std::vector< std::vector<typeT> >& means )
    {
        std::vector< std::vector<int> > sums;
        std::vector< std::vector<int> > cnts;

        for ( std::size_t k = 0 ; k < means.size() ; ++k ) {
            sums.push_back(std::vector<int>());
            cnts.push_back(std::vector<int>());
            for ( std::size_t l = 0 ; l < means[k].size() ; ++l ){
                sums[k].push_back(0);
                cnts[k].push_back(0);
            }
        }

        bool changed = true;
        while ( changed ) {
            changed = false;
            for ( std::size_t r = 0 ; r < src.height() ; ++r ) {
                const typeT* src_ptr = src.ptr(r);
                typeT* dst_ptr = dst.ptr(r);
                for ( std::size_t c = 0 ; c < src.width() ; ++c ) {
                    int min_diff = std::numeric_limits<int>::max();
                    int new_k = 0;
                    for ( std::size_t k = 0 ; k < means.size() ; ++k ) {
                        int dist = 0;
                        for ( std::size_t l = 0 ; l < src.channel() ; ++l ) {
                            typeT val = src_ptr[ c * src.channel() + l ];
                            int diff = val - means[k][l];
                            dist += ( diff * diff );
                        }
                        if ( dist < min_diff ) {
                            min_diff = dist;
                            new_k = k;
                        }
                    }
                    if ( dst_ptr[ c * dst.channel()] != new_k ) {
                        dst_ptr[ c * dst.channel()] = new_k;
                        changed = true;
                    }
                    for ( std::size_t l = 0 ; l < src.channel() ; ++l ) {
                        const typeT val = src_ptr[ c * src.channel() + l ];
                        sums[new_k][l] += val;
                        cnts[new_k][l] += 1;
                    }
                }
            }
            for ( std::size_t k = 0 ; k < means.size() ; ++k ) {
                for ( std::size_t l = 0 ; l < src.channel() ; ++l ) {
                    means[k][l] = sums[k][l] / cnts[k][l];
                    sums[k][l] = 0;
                    cnts[k][l] = 0;
                }
            }
        }

        for ( std::size_t r = 0 ; r < src.height() ; ++r ) {
            typeT* dst_ptr = dst.ptr(r);
            for ( std::size_t c = 0 ; c < src.width() ; ++c ) {
                int k = dst_ptr[c*dst.channel()];
                for ( std::size_t l = 0 ; l < src.channel() ; ++l ) {
                    dst_ptr[ c * dst.channel() + l ] = means[k][l];
                }
            }
        }
    }

}}


#endif
