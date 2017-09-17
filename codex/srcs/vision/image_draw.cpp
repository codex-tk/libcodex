#include <codex/vision/image_draw.hpp>

namespace codex { namespace vision {

    void line_to( image& img
              , const codex::vision::point& from
              , const codex::vision::point& to
              , const uint8_t val
              )
    {
        /*
        if ( ( from.x == to.x ) && ( from.y == to.y )) {
            img.at(from.x,from.y) = color;
        } else if ( from.x == to.x ) {
            for ( int y = std::min(from.y,to.y)
                  ; y < std::max(from.y,to.y)
                  ; ++y ) {
                img.at(from.x,y) = color;
            }
        } else if ( from.y == to.y ) {
            for ( int x = std::min(from.x,to.x)
                  ; x < std::max(from.x,to.x)
                  ; ++x ) {
                img.at(x,from.y) = color;
            }
        } else {
            double coefficient = ( from.y - to.y )/(double)(from.x - to.x );
            for ( int x = std::min(from.x,to.x)
                  ; x < std::max(from.x,to.x)
                  ; ++x ) {
                img.at(x,static_cast<int>(coefficient*x)) = color;
            }
        }*/

        int x1 = from.x;
        int y1 = from.y;
        int x2 = to.x;
        int y2 = to.y;

        int xs, ys, xe, ye;
        if (x1 == x2)
        {
            if (y1 < y2) {ys = y1; ye = y2;}
            else		 {ys = y2; ye = y1;}
            for (int r=ys ; r<=ye ; r++)
            {
                img.at(x1, r) = val;
            }
            return;
        }

        double a = (double)(y2-y1)/(x2-x1);
        int nHeight = img.height();

        if ((a>-1) && (a<1))
        {
            if (x1 < x2) {xs = x1; xe = x2; ys = y1; ye = y2;}
            else		 {xs = x2; xe = x1; ys = y2; ye = y1;}
            for (int c=xs ; c<=xe ; c++)
            {
                int r = (int)(a*(c-xs) + ys + 0.5);
                if (r<0 || r>=nHeight)
                    continue;
                img.at(c, r) = val;
            }
        }
        else
        {
            double invA = 1.0 / a;
            if (y1 < y2) {ys = y1; ye = y2; xs = x1; xe = x2; }
            else		 {ys = y2; ye = y1; xs = x2; xe = x1; }
            for (int r=ys ; r<=ye ; r++)
            {
                int c = (int)(invA*(r-ys) + xs + 0.5);
                if (r<0 || r>=nHeight)
                    continue;
                img.at(c, r) = val;
            }
        }
    }

}}
