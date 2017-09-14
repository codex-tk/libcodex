#ifndef __codex_vision_image_h__
#define __codex_vision_image_h__

#include <cassert>

#include <codex/codex.hpp>
#include <codex/vision/vision.hpp>

namespace codex { namespace vision {

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
            : _width(rhs.width()) , _height(rhs.height()) , _channel(rhs.channel())
            , _stride(((rhs.width() * rhs.channel() * sizeof(typeT)+3) & ~3 )/sizeof(typeT))
            , _buffer( _height * _stride )
        {
            for ( std::size_t r = 0 ; r < _height ; ++r )
            {
                const otherT* src = rhs.ptr(r);
                typeT* dst = this->ptr(r);
                for ( std::size_t c = 0 ; c < _width ; ++c ) {
                    std::size_t idx = c * _channel;
                    for ( std::size_t l = 0  ; l < _channel ; ++l )
                    {
                        dst[idx + l ] = codex::vision::operation< typeT , otherT>::clip( src[idx + l] );
                    }
                }
            }
        }

        image_base<typeT> get_channel( std::size_t l ) {
            if ( l > _channel ) {
                return image_base<typeT>();
            }
            image_base<typeT> dst( width() , height() );
            for ( std::size_t r = 0; r < _height ; ++r ) {
                typeT* src_ptr = this->ptr(r);
                typeT* dst_ptr = dst.ptr(r);
                for ( std::size_t c = 0 ; c < _width ; ++c ) {
                    dst_ptr[c] = src_ptr[ c * _channel + l];
                }
            }
            return dst;
        }

        template < typename otherT >
        void put_channnel( std::size_t l , const image_base<otherT>& src , std::size_t src_channel = 0 ) {
            if ( l > _channel ) {
                return ;
            }
            if ( src_channel > src.channel()){
                return ;
            }

            for ( std::size_t r = 0; r < std::min( src.height(), _height) ; ++r ) {
                const otherT* src_ptr = src.ptr(r);
                typeT* dst_ptr = this->ptr(r);
                for ( std::size_t c = 0 ; c < std::min( src.width(), _width) ; ++c ) {
                    dst_ptr[c * _channel + l] = src_ptr[ c * src.channel() + src_channel];
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
            if ( x >= 0 && x < width() && y >= 0 && y < height())
                return _buffer[ y * _stride + x * _channel + ch ];
            static typeT sample;
            return sample;
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

        void reset_channel( std::size_t l , typeT val ){
            if ( l >= channel())
                return;
            for ( std::size_t r = 0 ; r < height() ; ++r ){
                typeT* ptr = ptr(r);
                for ( std::size_t c = 0 ; c < width() ; ++c ) {
                    ptr[ c * channel() + l ] = val;
                }
            }
        }

        void reset_channels( std::vector< typeT >& values ) {
            for ( std::size_t r = 0 ; r < height() ; ++r ){
                typeT* ptr = this->ptr(r);
                for ( std::size_t c = 0 ; c < width() ; ++c ) {
                    for ( std::size_t l = 0
                          ; l < std::min( channel() , values.size() )
                          ; ++l ) {
                        ptr[ c * channel() + l ] = values[l];
                    }
                }
            }
        }

        template < typename scalarT >
        image_base& operator+=( const scalarT val );

        template < typename scalarT >
        image_base& operator-=( const scalarT val );

        template < typename otherT , typename otherAllocator >
        image_base& operator+=( const image_base<otherT,otherAllocator>& rhs );

        template < typename otherT  , typename otherAllocator >
        image_base& operator-=( const image_base<otherT,otherAllocator>& rhs );

        template < typename handlerT >
        void for_each( const handlerT& handler ) const {
            for ( std::size_t r = 0; r < height() ; ++r ) {
                const typeT* buf_ptr = ptr(r);
                for ( std::size_t c = 0 ; c < width() ; ++c ) {
                    for ( std::size_t l = 0 ; l < channel() ; ++l ) {
                        handler( r , c , l , buf_ptr[ c * channel() + l ] );
                    }
                }
            }
        }

        image_base transpose(void) {
            image_base dst( height() , width() , channel() );
            for ( std::size_t r = 0 ; r < height() ; ++r ) {
                typeT* src_ptr = ptr(r);
                for ( std::size_t c = 0 ; c < width() ; ++c ) {
                    typeT* dst_ptr = dst.ptr(c);
                    for ( std::size_t l = 0 ; l < channel() ; ++l ) {
                        dst_ptr[r * channel() + l ] = src_ptr[c * channel() + l];
                    }
                }
            }
            return dst;
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
        template < typename operand0T , typename operand1T , typename ansT , typename handlerT >
        void solve(  const image_base<operand0T>& operand0
                     , const image_base<operand1T>& operand1
                     , image_base< ansT >& ans
                     , const handlerT& operation )
        {
            assert( (operand0.width() == operand1.width()) && (operand1.width() == ans.width()));
            assert( (operand0.height() == operand1.height()) && (operand1.height() == ans.height()));

            for ( std::size_t r = 0 ; r < operand0.height() ; ++r ) {
                const operand0T* op0_ptr = operand0.ptr(r);
                const operand1T* op1_ptr = operand1.ptr(r);
                ansT* ans_ptr = ans.ptr(r);
                for ( std::size_t c = 0 ; c < operand0.width() ; ++c ) {
                    for ( std::size_t l = 0; l < ans.channel() ; ++l ) {
                        operand0T op0v = operand0T();
                        operand1T op1v = operand1T();
                        if ( operand0.channel() > l ) {
                            op0v = op0_ptr[ c * operand0.channel() + l];
                        }
                        if ( operand1.channel() > l ) {
                            op1v = op1_ptr[ c * operand1.channel() + l];
                        }
                        ans_ptr[c * ans.channel() + l] = operation( op0v , op1v );
                    }
                }
            }
        }
        template < typename operand0T , typename operand1T , typename ansT , typename handlerT >
        void solve(  const image_base<operand0T>& operand0
                     , const operand1T val
                     , image_base< ansT >& ans
                     , const handlerT& operation )
        {
            assert( (operand0.width() == ans.width()) && (operand0.height() == ans.height()));

            for ( std::size_t r = 0 ; r < operand0.height() ; ++r ) {
                const operand0T* op0_ptr = operand0.ptr(r);
                ansT* ans_ptr = ans.ptr(r);
                for ( std::size_t c = 0 ; c < operand0.width() ; ++c ) {
                    for ( std::size_t l = 0; l < ans.channel() ; ++l ) {
                        operand0T op0v = operand0T();
                        if ( operand0.channel() > l ) {
                            op0v = op0_ptr[ c * operand0.channel() + l];
                        }
                        ans_ptr[c * ans.channel() + l] = operation( op0v , val );
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
        detail::solve( src0 , src1 , dst , &codex::vision::operation<typeT,typeT>::add);
    }

    template < typename typeT >
    void add( const image_base<typeT>& src0
              , const typeT val
              , image_base<typeT>& dst  )
    {
        detail::solve( src0 , val , dst , &codex::vision::operation<typeT,typeT>::add );
    }


    template < typename typeT >
    void sub( const image_base<typeT>& src0
              , const image_base<typeT>& src1
              , image_base<typeT>& dst  )
    {
        detail::solve( src0 , src1 , dst , &codex::vision::operation<typeT,typeT>::sub );
    }

    template < typename typeT >
    void sub( const image_base<typeT>& src0
              , const typeT val
              , image_base<typeT>& dst  )
    {
        detail::solve( src0 , val , dst , &codex::vision::operation<typeT,typeT>::sub );
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

    template < typename typeT , typename Allocator >
    template < typename scalarT >
    image_base<typeT,Allocator>& image_base<typeT,Allocator>::operator+=( const scalarT val )  {
        detail::solve( *this , val , *this , &codex::vision::operation<typeT,scalarT>::add);
        return *this;
    }
    template < typename typeT  , typename Allocator >
    template < typename scalarT >
    image_base<typeT,Allocator>& image_base<typeT,Allocator>::operator-=( const scalarT val )  {
        detail::solve( *this , val , *this , &codex::vision::operation<typeT,scalarT>::sub);
        return *this;
    }

    template < typename typeT , typename Allocator >
    template < typename otherT , typename otherAllocator>
    image_base<typeT,Allocator>& image_base<typeT,Allocator>::operator+=( const image_base<otherT,otherAllocator>& rhs )  {
        detail::solve( *this , rhs , *this , &codex::vision::operation<typeT,otherT>::add);
        return *this;
    }
    template < typename typeT , typename Allocator >
    template < typename otherT , typename otherAllocator>
    image_base<typeT,Allocator>& image_base<typeT,Allocator>::operator-=( const image_base<otherT,otherAllocator>& rhs )  {
        detail::solve( *this , rhs , *this , &codex::vision::operation<typeT,otherT>::sub);
        return *this;
    }

}}

#endif
