#ifndef __codex_vision_canny_h__
#define __codex_vision_canny_h__

#include <codex/vision/vision.hpp>
#include <codex/vision/image_proc.hpp>

namespace codex { namespace vision {

void canny_edge( const image& src , image& dst , uint8_t threshold_hi , uint8_t threashold_lo );

}}

#endif
