#include <codex/vision/harris.hpp>

namespace codex { namespace vision {
namespace detail {
    void sobel_derivative_and_calc_product( const image& src
                                          , image_base<double>& dxx
                                          , image_base<double>& dxy
                                          , image_base<double>& dyy)
    {
        for ( std::size_t r = 1 ; r < src.height() - 1 ; ++r ) {

            double* dxx_ptr = dxx.ptr(r);
            double* dxy_ptr = dxy.ptr(r);
            double* dyy_ptr = dyy.ptr(r);

            const uint8_t* src_ptr_prev    = src.ptr(r-1);
            const uint8_t* src_ptr         = src.ptr(r);
            const uint8_t* src_ptr_next    = src.ptr(r+1);

            for ( std::size_t c = 1 ; c < src.width() - 1 ; ++c ) {
                double dx  = (  src_ptr_prev[c - 1]   * codex::vision::sobel_x[0] +
                                src_ptr_prev[c + 1]   * codex::vision::sobel_x[2] +
                                src_ptr[c - 1]        * codex::vision::sobel_x[3] +
                                src_ptr[c + 1]        * codex::vision::sobel_x[5] +
                                src_ptr_next[c - 1]   * codex::vision::sobel_x[6] +
                                src_ptr_next[c + 1]   * codex::vision::sobel_x[8]);
                double dy  = (  src_ptr_prev[c - 1]   * codex::vision::sobel_y[0] +
                                src_ptr_prev[c ]      * codex::vision::sobel_y[1] +
                                src_ptr_prev[c + 1]   * codex::vision::sobel_y[2] +
                                src_ptr_next[c - 1]   * codex::vision::sobel_y[6] +
                                src_ptr_next[c ]      * codex::vision::sobel_y[7] +
                                src_ptr_next[c + 1]   * codex::vision::sobel_y[8]);

                dxx_ptr[c] = dx * dx;
                dxy_ptr[c] = dx * dy;
                dyy_ptr[c] = dy * dy;
            }
        }
    }

    void corner_score( const image_base<double>&  dxx
                       , const image_base<double>&  dxy
                       , const image_base<double>&  dyy
                       , double k
                       , image_base<double>& score)
    {
        std::size_t height =  dxx.height();
        std::size_t width =  dxx.width();
        for ( std::size_t r = 1 ; r < height - 1; ++r ) {
            const double*  dxx_ptr =  dxx.ptr(r);
            const double*  dxy_ptr =  dxy.ptr(r);
            const double*  dyy_ptr =  dyy.ptr(r);
            double* score_ptr = score.ptr(r);
            for ( std::size_t c = 1 ; c < width - 1 ; ++c  ){
                score_ptr[c] = (  dxx_ptr[c] *  dyy_ptr[c] -  dxy_ptr[c] * dxy_ptr[c] )
                        - k * ( dxx_ptr[c] + dyy_ptr[c] ) * ( dxx_ptr[c] + dyy_ptr[c] );
            }
        }
    }

    void non_maximum_supression( const image_base<double>& score
                                 , double corner_threshold
                                 , std::vector<point>& points )
    {
        std::size_t height = score.height();
        std::size_t width = score.width();
        for ( std::size_t r = 2 ; r < height - 2; ++r ) {

            const double* score_ptr_prev = score.ptr(r-1);
            const double* score_ptr      = score.ptr(r);
            const double* score_ptr_next = score.ptr(r+1);

            for ( std::size_t c = 2 ; c < width - 2 ; ++c  ){
                if ( score_ptr[c] > corner_threshold ) {
                    if ( score_ptr[c] > score_ptr_prev[c] &&
                         score_ptr[c] > score_ptr_next[c] &&
                         score_ptr[c] > score_ptr[c-1] &&
                         score_ptr[c] > score_ptr[c+1] )
                    {
                        points.push_back( { static_cast<int>(c) , static_cast<int>(r) } );
                    }
                }
            }
        }
    }
}

void harris_corner( const image& src
  , double corner_threshold
  , double k
  , std::vector< codex::vision::point >& points )
{
  image_base<double> dxx( src.width() , src.height() );
  image_base<double> dxy( src.width() , src.height() );
  image_base<double> dyy( src.width() , src.height() );

  detail::sobel_derivative_and_calc_product(src, dxx , dxy , dyy );

  image_base<double> gssdxx( src.width() , src.height() );
  image_base<double> gssdxy( src.width() , src.height() );
  image_base<double> gssdyy( src.width() , src.height() );

  codex::vision::gaussian( dxx , gssdxx , 5 );
  codex::vision::gaussian( dxy , gssdxy , 5 );
  codex::vision::gaussian( dyy , gssdyy , 5 );

  image_base<double> score(src.width(),src.height());

  detail::corner_score( gssdxx , gssdxy , gssdyy , k , score );
  detail::non_maximum_supression( score , corner_threshold , points );
}

}}
