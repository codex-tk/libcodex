#ifndef __codex_vision_soble_h__
#define __codex_vision_soble_h__

#include <codex/vision/vision.hpp>
#include <codex/vision/kernel.hpp>

namespace codex { namespace vision {

    template < typename srcT , typename dstT >
    void sobel_edge( const image_base<srcT>& src , image_base<dstT>& dst ){
        std::size_t channel = src.channel();
        for ( std::size_t r = 1 ; r < src.height() - 1 ; ++r ) {
            dstT* dst_ptr = dst.ptr(r);
            const srcT* src_ptr_prev = src.ptr(r-1);
            const srcT* src_ptr = src.ptr(r);
            const srcT* src_ptr_next = src.ptr(r+1);
            for ( std::size_t c = 1 ; c < src.width() - 1 ; ++c ) {
                std::size_t c_offset = c * channel;
                for ( std::size_t l = 0 ; l < channel ; ++ l ) {
                    std::size_t c_idx = c_offset + l;
                    double x   = (  src_ptr_prev[c_idx-channel]   * codex::vision::sobel_x[0] +
                                    src_ptr_prev[c_idx+channel]   * codex::vision::sobel_x[2] +
                                    src_ptr[c_idx-channel]        * codex::vision::sobel_x[3] +
                                    src_ptr[c_idx+channel]        * codex::vision::sobel_x[5] +
                                    src_ptr_next[c_idx-channel]   * codex::vision::sobel_x[6] +
                                    src_ptr_next[c_idx+channel]   * codex::vision::sobel_x[8]);
                    double y   = (  src_ptr_prev[c_idx-channel]   * codex::vision::sobel_y[0] +
                                    src_ptr_prev[c_idx]           * codex::vision::sobel_y[1] +
                                    src_ptr_prev[c_idx+channel]   * codex::vision::sobel_y[2] +
                                    src_ptr_next[c_idx-channel]   * codex::vision::sobel_y[6] +
                                    src_ptr_next[c_idx]           * codex::vision::sobel_y[7] +
                                    src_ptr_next[c_idx+channel]   * codex::vision::sobel_y[8]);
                    dst_ptr[c_idx] = std::sqrt( (x * x + y * y) / 32 );
                }
            }
        }
    }

}}

#endif
