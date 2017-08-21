#ifndef __codex_vision_image_proc_h__
#define __codex_vision_image_proc_h__

#include <codex/vision/image.hpp>

namespace codex { namespace vision {

    template < typename typeT >
    image gray_scale( const image_base<typeT>& src ){
        image ans( src.width() , src.height());
        for ( std::size_t y = 0 ; y < src.height() ; ++y ){
            for ( std::size_t x = 0 ; x < src.width() ; ++x ) {
                int val = 0;
                for ( std::size_t c = 0 ; c < src.channel() ; ++c ) {
                    val += src.at(x,y,c);
                }
                ans.at(x,y) = (val/src.channel());
            }
        }
        return ans;
    }

}}


#endif
