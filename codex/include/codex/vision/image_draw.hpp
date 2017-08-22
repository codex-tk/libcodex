#ifndef __codex_vision_image_draw_h__
#define __codex_vision_image_draw_h__

#include <codex/vision/image.hpp>

namespace codex { namespace vision {

    void line_to( image& img
                  , const codex::vision::point& from
                  , const codex::vision::point& to
                  , const uint8_t color
                  );

}}

#endif
