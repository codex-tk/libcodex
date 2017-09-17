#include <codex/vision/canny.hpp>

namespace codex { namespace vision {

namespace detail {
    void sobel_derivative_and_calc_angle( const image& gss_img
                                          , image_base<double>& derivative
                                          , image& angle
                                          , uint8_t threshold_lo)
    {
        double rad2deg = 180.0 / M_PI;

        for ( std::size_t r = 1 ; r < gss_img.height() - 1 ; ++r ) {
            double* derivative_ptr = derivative.ptr(r);
            uint8_t* angle_ptr = angle.ptr(r);
            const uint8_t* gss_img_ptr_prev    = gss_img.ptr(r-1);
            const uint8_t* gss_img_ptr         = gss_img.ptr(r);
            const uint8_t* gss_img_ptr_next    = gss_img.ptr(r+1);

            for ( std::size_t c = 1 ; c < gss_img.width() - 1 ; ++c ) {
                double x   = (  gss_img_ptr_prev[c - 1]   * codex::vision::sobel_x[0] +
                                gss_img_ptr_prev[c + 1]   * codex::vision::sobel_x[2] +
                                gss_img_ptr[c - 1]        * codex::vision::sobel_x[3] +
                                gss_img_ptr[c + 1]        * codex::vision::sobel_x[5] +
                                gss_img_ptr_next[c - 1]   * codex::vision::sobel_x[6] +
                                gss_img_ptr_next[c + 1]   * codex::vision::sobel_x[8]);
                double y   = (  gss_img_ptr_prev[c - 1]   * codex::vision::sobel_y[0] +
                                gss_img_ptr_prev[c ]      * codex::vision::sobel_y[1] +
                                gss_img_ptr_prev[c + 1]   * codex::vision::sobel_y[2] +
                                gss_img_ptr_next[c - 1]   * codex::vision::sobel_y[6] +
                                gss_img_ptr_next[c ]      * codex::vision::sobel_y[7] +
                                gss_img_ptr_next[c + 1]   * codex::vision::sobel_y[8]);


                derivative_ptr[c] = std::sqrt(x * x + y * y );
                if ( derivative_ptr[c] >= threshold_lo ){
                    if( x == 0 ) {
                        angle_ptr[c] = 90;
                    } else {
                        double theta = std::atan2( y , x ) * rad2deg;
                        if ( ( theta >= -22.5 && theta < 22.5 ) ||
                             ( theta >= 157.5 && theta < -157.5 )) {
                             angle_ptr[c] = 0;
                        } else if (( theta >= 22.5 && theta < 67.5 ) ||
                                   ( theta >= -157.5 && theta < -112.5)) {
                            angle_ptr[c] = 45;
                        } else if (( theta >= 67.5 && theta < 112.5 ) ||
                                   ( theta >= -112.5 && theta < -67.5 )) {
                            angle_ptr[c] = 90;
                        } else {
                            angle_ptr[c] = 135;
                        }
                    }
                } else {
                    angle_ptr[c]= 255;
                }
            }
        }
    }

    void non_maximum_supression( const image_base<double>& derivative
                                 , const image& angle
                                 , image& sup )
    {
        for ( std::size_t r = 1 ; r < derivative.height() - 1 ; ++r ) {
            const double* derivative_ptr_prev = derivative.ptr(r-1);
            const double* derivative_ptr      = derivative.ptr(r);
            const double* derivative_ptr_next = derivative.ptr(r+1);
            const uint8_t* angle_ptr = angle.ptr(r);
            uint8_t* sup_ptr = sup.ptr(r);
            for ( std::size_t c = 1 ; c < derivative.width() - 1 ; ++c ) {
                switch (angle_ptr[c]) {
                case 0:
                    if ((derivative_ptr[c] > derivative_ptr[ c - 1 ])&&
                        (derivative_ptr[c] > derivative_ptr[ c + 1 ]))
                    {
                        sup_ptr[c] = 255;
                    }
                    break;
                case 45:
                    if ((derivative_ptr[c] > derivative_ptr_prev[ c + 1 ])&&
                        (derivative_ptr[c] > derivative_ptr_next[ c - 1 ]))
                    {
                        sup_ptr[c] = 255;
                    }
                    break;
                case 90:
                    if ((derivative_ptr[c] > derivative_ptr_prev[c])&&
                        (derivative_ptr[c] > derivative_ptr_next[c]))
                    {
                        sup_ptr[c] = 255;
                    }
                    break;;
                case 135:
                    if ((derivative_ptr[c] > derivative_ptr_prev[ c - 1 ])&&
                        (derivative_ptr[c] > derivative_ptr_next[ c + 1 ]))
                    {
                        sup_ptr[c] = 255;
                    }
                    break;;
                default:
                    break;
                }
            }
        }
    }

    void hysteresis(  const image_base<double>& derivative
                      , const image& angle
                      , const image& sup
                      , image& dst
                      , uint8_t threshold_hi
                      , uint8_t threshole_lo )
    {
        for ( std::size_t r = 1 ; r < derivative.height() - 1 ; ++r ) {
            const double* derivative_ptr_prev = derivative.ptr(r-1);
            const double* derivative_ptr      = derivative.ptr(r);
            const double* derivative_ptr_next = derivative.ptr(r+1);
            const uint8_t* angle_ptr = angle.ptr(r);
            const uint8_t* sup_ptr = sup.ptr(r);
            uint8_t* dst_ptr = dst.ptr(r);
            for ( std::size_t c = 1 ; c < derivative.width() - 1 ; ++c ) {
                if ( sup_ptr[c] == 255 ) {
                    if ( derivative_ptr[c] >= threshold_hi ) {
                        dst_ptr[c] = 255;
                    } else if ( derivative_ptr[c] >= threshole_lo) {
                        switch (angle_ptr[c]) {
                        case 0:
                            if ((derivative_ptr[ c - 1 ] >= threshold_hi) ||
                                (derivative_ptr[ c + 1 ] >= threshold_hi))
                            {
                                dst_ptr[c] = 255;
                            }
                            break;
                        case 45:
                            if ((derivative_ptr_prev[ c + 1 ] >= threshold_hi) ||
                                (derivative_ptr_next[ c - 1 ] >= threshold_hi))
                            {
                                dst_ptr[c] = 255;
                            }
                            break;
                        case 90:
                            if (( derivative_ptr_prev[c] >= threshold_hi) ||
                                ( derivative_ptr_next[c] >= threshold_hi))
                            {
                                dst_ptr[c] = 255;
                            }
                            break;;
                        case 135:
                            if (( derivative_ptr_prev[ c - 1 ] >= threshold_hi) ||
                                ( derivative_ptr_next[ c + 1 ] >= threshold_hi))
                            {
                                dst_ptr[c] = 255;
                            }
                            break;;
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }
}

void canny_edge( const image& src , image& dst , uint8_t threshold_hi , uint8_t threshold_lo ){
    assert( src.channel() == 1 );

    codex::vision::image gss_img(src.width(),src.height());
    codex::vision::gaussian( src , gss_img , 5 , 1 );

    codex::vision::image_base<double> derivative( gss_img.width(),gss_img.height());
    codex::vision::image angle( gss_img.width(),gss_img.height());

    detail::sobel_derivative_and_calc_angle( gss_img , derivative , angle , threshold_lo );

    //detail::non_maximum_supression( derivative , angle , dst );

    codex::vision::image non_maximum_sup_img( gss_img.width() , gss_img.height());

    detail::non_maximum_supression( derivative , angle , non_maximum_sup_img );
    detail::hysteresis( derivative , angle , non_maximum_sup_img , dst , threshold_hi , threshold_lo );

}

}}
