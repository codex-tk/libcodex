#ifndef __codex_vision_harris_h__
#define __codex_vision_harris_h__

#include <codex/vision/vision.hpp>
#include <codex/vision/image_proc.hpp>

namespace codex { namespace vision {

void harris_corner( const image& src
  , double corner_threshold // 1e8 ( 10^8 )
  , double k  // 0.04
  , std::vector< codex::vision::point >& points );

}}

#endif
