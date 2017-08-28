#ifndef __codex_vision_vision_h__
#define __codex_vision_vision_h__

#include <codex/codex.hpp>

namespace codex { namespace vision {

    struct point{
        int x;
        int y;
    };

    template < typename typeT , typename otherT >
    struct operation{
        static const typeT type_min = std::numeric_limits<typeT>::min();
        static const typeT type_max = std::numeric_limits<typeT>::max();

        //typedef typename std::conditional< sizeof(typeT)>=sizeof(otherT) , typeT , otherT >::type value_type;

        static typeT add( typeT v0 , otherT v1) {
            decltype ( v0 + v1 ) value = v0 + v1;
            return clip(value);
        }

        static typeT sub( typeT v0 , otherT v1) {
            decltype ( v0 - v1 ) value = v0 - v1;
            return clip(value);
        }

        template < typename bigT >
        static typeT clip( bigT value ) {
            return value < type_min ? type_min
                    : value > type_max ? type_max : value;
        }

        static typeT clip( typeT value ) {
            return value;
        }
    };
}}

#endif
