#ifndef __codex_vision_vision_h__
#define __codex_vision_vision_h__

#include <codex/codex.hpp>

namespace codex { namespace vision {

    struct point{
        int x;
        int y;
    };

    template < typename typeT >
    struct operation{
        static typeT add( typeT v0 , typeT v1) {
            return v0 + v1;
        }
        static typeT sub( typeT v0 , typeT v1) {
            return v0 - v1;
        }
    };

    template < >
    struct operation<uint8_t>{
        static uint8_t add( uint8_t v0 , uint8_t v1) {
            int val = static_cast<int>(v0) + static_cast<int>(v1);
            return val < 0 ? 0 : val > 0xff ? 0xff : val;
        }
        static uint8_t sub( uint8_t v0 , uint8_t v1) {
            int val = static_cast<int>(v0) - static_cast<int>(v1);
            return val < 0 ? 0 : val > 0xff ? 0xff : val;
        }
    };
}}

#endif
