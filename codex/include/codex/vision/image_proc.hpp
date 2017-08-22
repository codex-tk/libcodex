#ifndef __codex_vision_image_proc_h__
#define __codex_vision_image_proc_h__

#include <codex/vision/image.hpp>

namespace codex { namespace vision {

    template < typename typeT >
    image gray_scale( const image_base<typeT>& src  ){
        image dst( src.width() , src.height());
        for ( std::size_t y = 0 ; y < src.height() ; ++y ){
            for ( std::size_t x = 0 ; x < src.width() ; ++x ) {
                int val = 0;
                for ( std::size_t c = 0 ; c < src.channel() ; ++c ) {
                    val += src.at(x,y,c);
                }
                dst.at(x,y) = (val/src.channel());
            }
        }
        return dst;
    }

    void histogram_equation( const image& src , image& dst );
    void histogram_equation_debug( const image& src
                                   , image& dst
                                   , image& orig_hist
                                   , image& equation_hist );
    void histogram_graph( const image& src , image& dst );


    template < typename typeT >
    void add( const image_base<typeT>& src0
              , const image_base<typeT>& src1
              , image_base<typeT>& dst  )
    {
        /*
        cassert( src0.width() == src1.width());
        cassert( src1.width() == dst.width());
        cassert( src0.height() == src1.height());
        cassert( src1.height() == dst.height());
        cassert( src0.channel() == src1.channel());
        cassert( src1.channel() == dst.channel());
        */
        for ( std::size_t y = 0 ; y < src0.height() ; ++y ) {
            for ( std::size_t x = 0 ; x < src0.width() ; ++x ) {
                for ( std::size_t c = 0; c < src0.channel() ; ++c ) {
                    dst.at(x,y,c) = codex::vision::operation<typeT>::add( src0.at(x,y,c) , src1.at(x,y,c));
                }
            }
        }
    }

    template < typename typeT >
    void sub( const image_base<typeT>& src0
              , const image_base<typeT>& src1
              , image_base<typeT>& dst  )
    {
        /*
        cassert( src0.width() == src1.width());
        cassert( src1.width() == dst.width());
        cassert( src0.height() == src1.height());
        cassert( src1.height() == dst.height());
        cassert( src0.channel() == src1.channel());
        cassert( src1.channel() == dst.channel());
        */
        for ( std::size_t y = 0 ; y < src0.height() ; ++y ) {
            for ( std::size_t x = 0 ; x < src0.width() ; ++x ) {
                for ( std::size_t c = 0; c < src0.channel() ; ++c ) {
                    dst.at(x,y,c) = codex::vision::operation<typeT>::sub( src0.at(x,y,c) , src1.at(x,y,c));
                }
            }
        }
    }

    template < typename typeT >
    image_base<typeT> operator+( const image_base<typeT>& src0
                                 , const image_base<typeT>& src1 )
    {
        image_base<typeT> dst( src0.width() , src0.height() , src0.channel());
        add( src0 , src1 , dst );
        return dst;
    }

    template < typename typeT >
    image_base<typeT> operator-( const image_base<typeT>& src0
                                 , const image_base<typeT>& src1 )
    {
        image_base<typeT> dst( src0.width() , src0.height() , src0.channel());
        sub( src0 , src1 , dst );
        return dst;
    }

    /*
    namespace detail{
        template < typename typeT , typename handlerT >
        image_base<typeT> operation_impl( const image_base<typeT>& src0
                                          , const image_base<typeT>& src1
                                          , const handlerT& handler )
        {
            image_base<typeT> dst( src0.width() , src0.height());
            for ( std::size_t r = 0 ; r < src0.height(); ++r ){
                typeT* dst_ptr = dst.ptr(r);
                const typeT* src0_ptr = src0.ptr(r);
                const typeT* src1_ptr = src1.ptr(r);
                for ( std::size_t c = 0 ; c < src0.width() ; ++c ) {
                    dst_ptr[c] = handler( src0_ptr[c] , src1_ptr[c]);
                }
            }
            return dst;
        }
    }

    template < typename typeT >
    image_base<typeT> operator+( const image_base<typeT>& src0
                                 , const image_base<typeT>& src1 )
    {
        return detail::operation_impl( src0 , src1 , [] ( typeT a ,typeT b ) -> typeT {
            decltype ( a + b ) val = a + b;
            if ( val < 0 ) return 0;
            if ( val > std::numeric_limits<typeT>::max())
                return std::numeric_limits<typeT>::max();
            return val;
        });
    }

    template < typename typeT >
    image_base<typeT> operator-( const image_base<typeT>& src0
                                 , const image_base<typeT>& src1 )
    {
        return detail::operation_impl( src0 , src1 , [] ( typeT a ,typeT b ) {
            decltype ( a - b ) val = a - b;
            if ( val < 0 ) return 0;
            if ( val > std::numeric_limits<typeT>::max())
                return std::numeric_limits<typeT>::max();
            return val;
        });
    }
    */


}}


#endif
