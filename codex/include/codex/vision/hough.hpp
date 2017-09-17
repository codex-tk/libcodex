#ifndef __codex_vision_hough_h__
#define __codex_vision_hough_h__

#include <codex/vision/vision.hpp>
#include <codex/vision/image_proc.hpp>

namespace codex { namespace vision {

struct hough_line_result{
    int rho;
    int theta;
    int vote;
    int width;

    codex::vision::point from( void ) const;
    codex::vision::point to( void ) const;
};

void hough_line( const image& src , int num_of_cross_lines
                 , uint8_t edge_threashold , double res_theta
                 , std::vector<hough_line_result>& lines
                 );


void hough_line( const image& src , int num_of_cross_lines
                 , uint8_t edge_threashold , double res_theta
                 , std::vector<hough_line_result>& lines
                 , image& param_out
                 );

}}

#endif
