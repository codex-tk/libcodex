#ifndef __codex_vision_image_h__
#define __codex_vision_image_h__

#include <cassert>

#include <codex/codex.hpp>
#include <codex/vision/vision.hpp>



namespace codex { namespace vision {

/**
    template < typename typeT >
    typeT pixel_cast( typeT src ) {
        return src;
    }

    template < typename typeT , typename otherT >
    typeT pixel_cast( otherT src ) {
        return src < 0 ? 0 : src > std::numeric_limits<typeT>::max() ?
                             std::numeric_limits<typeT>::max() : src;
    }
*/
    template < typename typeT , typename Allocator = std::allocator<typeT>>
    class image_base {
    public:
        typedef typeT value_type;
        image_base()
            : image_base(0,0)
        {

        }

        image_base( std::size_t width , std::size_t height , std::size_t channel = 1)
            : _width(width) , _height(height) , _channel(channel)
            , _stride(((width * channel * sizeof(typeT)+3) & ~3 )/sizeof(typeT))
            , _buffer( _height * _stride )
        {

        }

        image_base( const image_base& rhs )
            : _width(rhs._width) , _height(rhs._height) , _channel(rhs._channel)
            , _stride(rhs._stride)
            , _buffer( rhs._buffer )
        {

        }

        image_base( image_base&& rhs )
            : _width(rhs._width) , _height(rhs._height) , _channel(rhs._channel)
            , _stride(rhs._stride)
            , _buffer( std::move(rhs._buffer))
        {
        }

        image_base& operator=( const image_base& rhs )
        {
            _width = rhs._width;
            _height = rhs._height;
            _channel = rhs._channel;
            _stride = rhs._stride;
            _buffer = rhs._buffer;
            return *this;
        }

        image_base& operator=( image_base&& rhs )
        {
            _width = rhs._width;
            _height = rhs._height;
            _channel = rhs._channel;
            _stride = rhs._stride;
            _buffer = std::move(rhs._buffer);
            return *this;
        }

        template < typename otherT >
        image_base( const image_base< otherT >& rhs )
            : _width(rhs._width) , _height(rhs._height) , _channel(rhs._channel)
            , _stride(((rhs._width * rhs._channel * sizeof(typeT)+3) & ~3 )/sizeof(typeT))
            , _buffer( _height * _stride )
        {
            for ( int r = 0 ; r < _height ; ++r )
            {
                otherT* src = rhs.ptr(r);
                typeT* dst = this->ptr(r);
                for ( int c = 0 ; c < _width ; ++c ) {
                    dst[c] = codex::vision::operation< typeT >::convert( src[c] );
                }
            }
        }

        ~image_base( void ) {
        }

        std::size_t width(void) const { return _width; }
        std::size_t height(void) const { return _height; }
        std::size_t channel(void) const { return _channel; }
        std::size_t stride(void) const { return _stride; }

        typeT* ptr( std::size_t row = 0 , std::size_t col = 0) {
            return &(_buffer[ row * _stride + col * _channel ]);
        }

        const typeT* ptr(std::size_t row = 0, std::size_t col = 0) const {
            return &(_buffer[row * _stride + col * _channel]);
        }

        typeT& at( std::size_t x , std::size_t y , std::size_t ch = 0 ) {
            return _buffer[ y * _stride + x * _channel + ch ];
        }

        const typeT& at( std::size_t x , std::size_t y , std::size_t ch = 0 ) const {
            return _buffer[ y * _stride + x * _channel + ch ];
        }

        typeT at_r( std::size_t x , std::size_t y , std::size_t ch , typeT def_val ) const {
            if ( x >= 0 && x < width() && y >= 0 && y < height())
                return at(x,y,ch);
            return def_val;
        }

        void reset( typeT val ) {
            _buffer.assign( _buffer.size() , val );
        }
    private:
        std::size_t _width;
        std::size_t _height;
        std::size_t _channel;
        std::size_t _stride;
        std::vector<typeT , Allocator > _buffer;
    };

    typedef image_base<uint8_t> image;

    template < typename typeT >
    bool is_same_format( const image_base<typeT>& rhs , const image_base<typeT>& lhs ) {
        if ( rhs.width() == lhs.width()){
            if ( rhs.height() == lhs.height()){
                return rhs.channel() == lhs.channel();
            }
        }
        return false;
    }


    namespace detail {
        template < typename typeT , typename handlerT >
        void apply(  const image_base<typeT>& src0
                     , const image_base<typeT>& src1
                     , image_base<typeT>& dst
                     , const handlerT& handler )
        {
            assert( is_same_format( src0 , src1));
            assert( is_same_format( src1 , dst ));
            for ( std::size_t y = 0 ; y < src0.height() ; ++y ) {
                for ( std::size_t x = 0 ; x < src0.width() ; ++x ) {
                    for ( std::size_t c = 0; c < src0.channel() ; ++c ) {
                        dst.at(x,y,c) = handler(src0.at(x,y,c) , src1.at(x,y,c));
                    }
                }
            }
        }
        template < typename typeT , typename handlerT >
        void apply(  const image_base<typeT>& src0
                     , const typeT val
                     , image_base<typeT>& dst
                     , const handlerT& handler )
        {
            assert( is_same_format( src0 , dst));
            for ( std::size_t y = 0 ; y < src0.height() ; ++y ) {
                for ( std::size_t x = 0 ; x < src0.width() ; ++x ) {
                    for ( std::size_t c = 0; c < src0.channel() ; ++c ) {
                        dst.at(x,y,c) = handler(src0.at(x,y,c) , val);
                    }
                }
            }
        }
    }

    template < typename typeT >
    void add( const image_base<typeT>& src0
              , const image_base<typeT>& src1
              , image_base<typeT>& dst  )
    {
        detail::apply( src0 , src1 , dst , &codex::vision::operation<typeT>::add );
    }

    template < typename typeT >
    void add( const image_base<typeT>& src0
              , const typeT val
              , image_base<typeT>& dst  )
    {
        detail::apply( src0 , val , dst , &codex::vision::operation<typeT>::add );
    }


    template < typename typeT >
    void sub( const image_base<typeT>& src0
              , const image_base<typeT>& src1
              , image_base<typeT>& dst  )
    {
        detail::apply( src0 , src1 , dst , &codex::vision::operation<typeT>::sub );
    }

    template < typename typeT >
    void sub( const image_base<typeT>& src0
              , const typeT val
              , image_base<typeT>& dst  )
    {
        detail::apply( src0 , val , dst , &codex::vision::operation<typeT>::sub );
    }

    template < typename typeT >
    image_base<typeT> operator+( const image_base<typeT>& src0
                                 , const image_base<typeT>& src1 )
    {
        assert( is_same_format( src0 , src1));
        image_base<typeT> dst( src0.width() , src0.height() , src0.channel());
        add( src0 , src1 , dst );
        return dst;
    }

    template < typename typeT >
    image_base<typeT> operator+( const image_base<typeT>& src0
                                 , const typeT val)
    {
        image_base<typeT> dst( src0.width() , src0.height() , src0.channel());
        add( src0 , val , dst );
        return dst;
    }

    template < typename typeT >
    image_base<typeT> operator-( const image_base<typeT>& src0
                                 , const image_base<typeT>& src1 )
    {
        assert( is_same_format( src0 , src1));
        image_base<typeT> dst( src0.width() , src0.height() , src0.channel());
        sub( src0 , src1 , dst );
        return dst;
    }

    template < typename typeT >
    image_base<typeT> operator-( const image_base<typeT>& src0
                                 , const typeT val)
    {
        image_base<typeT> dst( src0.width() , src0.height() , src0.channel());
        sub( src0 , val , dst );
        return dst;
    }

    void disable_no_symbol();
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
