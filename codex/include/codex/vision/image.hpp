#ifndef __codex_vision_image_h__
#define __codex_vision_image_h__

#include <codex/codex.hpp>
#include <codex/vision/vision.hpp>

namespace codex { namespace vision {

    template < typename typeT >
    typeT pixel_cast( typeT src ) {
        return src;
    }

    template < typename typeT , typename otherT >
    typeT pixel_cast( otherT src ) {
        return src < 0 ? 0 : src > 0xff ? 0xff : src;
    }

    template < typename typeT , typename Allocator = std::allocator<typeT>>
    class image_base {
    public:
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
                    dst[c] = codex::vision::pixel_cast< typeT >( src[c] );
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
}}

#endif
