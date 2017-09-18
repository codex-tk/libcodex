#include <codex/vision/hough.hpp>


namespace codex { namespace vision {
namespace detail {
    const double deg2rad = M_PI / 180;
}
/*
    rho = x sin t + y con t;
    y = ( rho - x sin t ) / cos t
 * */

codex::vision::point hough_line_result::from( void ) const{
    if ( theta == 90 )  {
        return { rho , 0 };
    }
    return {  0 , static_cast<int>(rho /  cos( theta * detail::deg2rad )) };
}

codex::vision::point hough_line_result::to( void ) const {
    if ( theta == 90 )  {
        return { rho , width - 1 };
    }
    point p;
    p.x = width - 1;
    p.y =static_cast<int>(( rho - p.x * sin( theta * detail::deg2rad) )
                          / cos( theta * detail::deg2rad ));
    return p;
}

void hough_line( const image& src , int num_of_cross_lines
                 , uint8_t edge_threashold , double res_theta
                 , std::vector<hough_line_result>& lines
                 )
{
    int num_of_theta = 180 / res_theta;
    int num_of_rho = std::sqrt( src.width() * src.width() + src.height() *  src.height() );
    int num_of_rho2 = num_of_rho * 2;

    std::vector< double > sin_lut(num_of_theta);
    std::vector< double > cos_lut(num_of_theta);

    const double to_rad = M_PI / num_of_theta;

    for ( int i = 0 ; i < num_of_theta ; ++i ) {
        sin_lut[i] = std::sin( i * to_rad );
        cos_lut[i] = std::cos( i * to_rad );
    }

    image_base<int> acc( num_of_theta , num_of_rho2 );

    for ( int r = 0 ; r < static_cast<int>(src.height()) ; ++r ) {
        const uint8_t* src_ptr = src.ptr(r);
        for (int c = 0 ; c < static_cast<int>(src.width()) ; ++c ) {
            if ( src_ptr[c] > edge_threashold ) {
                for ( int theta_idx = 0 ; theta_idx < num_of_theta ; ++theta_idx ) {
                    int rho = static_cast<int>(  c * sin_lut[theta_idx]
                                               + r * cos_lut[theta_idx]
                                               + num_of_rho
                                               + 0.5);
                    acc.at( theta_idx , rho ) += 1;
                }
            }
        }
    }

    for ( int r = 0 ; r < static_cast<int>(acc.height()) ; ++r ) {
        int* acc_ptr = acc.ptr(r);
        for ( int c = 0 ; c < static_cast<int>(acc.width()) ; ++c ) {
            if ( acc_ptr[c] >= num_of_cross_lines ) {
                hough_line_result result;
                result.rho = r - num_of_rho;
                result.theta = c * res_theta;
                result.vote = acc_ptr[c];
                result.width = src.width();
                lines.push_back(result);
            }
        }
    }
}


void hough_line( const image& src , int num_of_cross_lines
                 , uint8_t edge_threashold , double res_theta
                 , std::vector<hough_line_result>& lines
                 , image& param_out
                 )
{
    int num_of_theta = 180 / res_theta;
    int num_of_rho = std::sqrt( src.width() * src.width() + src.height() *  src.height() );
    int num_of_rho2 = num_of_rho * 2;

    std::vector< double > sin_lut(num_of_theta);
    std::vector< double > cos_lut(num_of_theta);

    const double to_rad = M_PI / num_of_theta;

    for ( int i = 0 ; i < num_of_theta ; ++i ) {
        sin_lut[i] = std::sin( i * to_rad );
        cos_lut[i] = std::cos( i * to_rad );
    }

    image_base<int> acc( num_of_theta , num_of_rho2 );

    for ( int r = 0 ; r < static_cast<int>(src.height()) ; ++r ) {
        const uint8_t* src_ptr = src.ptr(r);
        for (int c = 0 ; c < static_cast<int>(src.width()) ; ++c ) {
            if ( src_ptr[c] > edge_threashold ) {
                for ( int theta_idx = 0 ; theta_idx < num_of_theta ; ++theta_idx ) {
                    int rho = static_cast<int>(  c * sin_lut[theta_idx]
                                               + r * cos_lut[theta_idx]
                                               + num_of_rho
                                               + 0.5);
                    acc.at( theta_idx , rho ) += 1;
                }
            }
        }
    }

    param_out = image( acc.width() , acc.height());
    for ( int r = 0 ; r < static_cast<int>(acc.height()) ; ++r ) {
        int* acc_ptr = acc.ptr(r);
        uint8_t* param_ptr = param_out.ptr(r);
        for ( int c = 0 ; c < static_cast<int>(acc.width()) ; ++c ) {
            param_ptr[c] = codex::vision::operation< uint8_t , int >::clip( acc_ptr[c] );
            if ( acc_ptr[c] >= num_of_cross_lines ) {
                hough_line_result result;
                result.rho = r - num_of_rho;
                result.theta = c * res_theta;
                result.vote = acc_ptr[c];
                result.width = src.width();
                lines.push_back(result);
            }
        }
    }
}


}}
