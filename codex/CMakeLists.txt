project( codex )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )



if ( MSVC )
	set(BOOST_ROOT "C:/Users/codex/works/boost_1_64_0")
	#set(BOOST_LIBRARYDIR "C:/Users/codex/works/boost_1_64_0/stage/lib")
    set( PLATFORM_DEPEND_HEADERS

    )
    set( PLATFORM_DEPEND_SRCS

    )
else()
  if ( APPLE )
    set( PLATFORM_DEPEND_HEADERS

    )
    set( PLATFORM_DEPEND_SRCS

    )
  else()
    set( PLATFORM_DEPEND_HEADERS
    )
    set( PLATFORM_DEPEND_SRCS
    )
  endif()
#common
  set( PLATFORM_DEPEND_HEADERS
      ${PLATFORM_DEPEND_HEADERS}
  )
  set( PLATFORM_DEPEND_SRCS
      ${PLATFORM_DEPEND_SRCS}
  )
endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)

find_package (Boost 1.60.0 REQUIRED COMPONENTS system date_time regex )

if (Boost_FOUND)
  include_directories (SYSTEM ${Boost_INCLUDE_DIR})
endif()

set( HEADERS
    include/codex/codex.hpp
    include/codex/function.hpp
    include/codex/function.ipp
  	include/codex/error_code.hpp
  	include/codex/conv/base64.hpp
  	include/codex/conv/checksum.hpp
  	include/codex/conv/crc32.hpp
  	include/codex/conv/string.hpp
		include/codex/singleton.hpp
    include/codex/vision/vision.hpp
    include/codex/vision/image.hpp
    include/codex/vision/bitmap.hpp
    include/codex/vision/image_proc.hpp
    include/codex/vision/kernel.hpp
    include/codex/vision/image_draw.hpp
    include/codex/vision/fft.hpp
    ${PLATFORM_DEPEND_HEADERS}
    )

set( SRCS
    srcs/codex.cpp
    srcs/error_code.cpp
    srcs/conv/base64.cpp
    srcs/conv/crc32.cpp
    srcs/conv/string.cpp
    srcs/vision/image.cpp
    srcs/vision/bitmap.cpp
    srcs/vision/image_proc.cpp
    srcs/vision/image_draw.cpp
    srcs/vision/fft.cpp
    ${PLATFORM_DEPEND_SRCS}
    )

  add_library( codex STATIC ${HEADERS} ${SRCS} )

if ( CODEX_SHARED )
  add_library( codex.shared SHARED ${HEADERS} ${SRCS} )
endif()
