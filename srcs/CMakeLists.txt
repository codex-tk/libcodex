project( codex )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

if ( MSVC )
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

set( HEADERS
    include/codex/codex.hpp
    include/codex/function.hpp
    include/codex/function.ipp
	include/codex/error_code.hpp
	include/codex/conv/base64.hpp
	include/codex/conv/checksum.hpp
	include/codex/conv/crc32.hpp
	include/codex/conv/string.hpp
    ${PLATFORM_DEPEND_HEADERS}
)

set( SRCS
    codex.cpp
	error_code.cpp
	conv/base64.cpp
	conv/crc32.cpp
	conv/string.cpp
    ${PLATFORM_DEPEND_SRCS}
    )

  add_library( codex STATIC ${HEADERS} ${SRCS} )

if ( CODEX_SHARED )
  add_library( codex.shared SHARED ${HEADERS} ${SRCS} )
endif()
