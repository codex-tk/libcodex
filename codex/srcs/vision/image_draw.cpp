#include <codex/vision/image_draw.hpp>

namespace codex { namespace vision {

    void line_to( image& img
              , const codex::vision::point& from
              , const codex::vision::point& to
              , const uint8_t color
              )
    {
        if ( ( from.x == to.x ) && ( from.y == to.y )) {
            img.at(from.x,from.y) = color;
        } else if ( from.x == to.x ) {
            for ( int y = std::min(from.y,to.y)
                  ; y < std::max(from.y,to.y)
                  ; ++y ) {
                img.at(from.x,y) = color;
            }
        }if ( from.y == to.y ) {
            for ( int x = std::min(from.x,to.x)
                  ; x < std::max(from.x,to.x)
                  ; ++x ) {
                img.at(x,from.y) = color;
            }
        } else {
            double slope = ( from.y - to.y )/(double)(from.x - to.x );
            for ( int x = std::min(from.x,to.x)
                  ; x < std::max(from.x,to.x)
                  ; ++x ) {
                img.at(x,static_cast<int>(slope*x)) = color;
            }
        }
    }

}}
