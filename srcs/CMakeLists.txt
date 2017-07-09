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
    ${PLATFORM_DEPEND_HEADERS}
)

set( SRCS
    codex.cpp
    ${PLATFORM_DEPEND_SRCS}
    )

  add_library( codex STATIC ${HEADERS} ${SRCS} )

if ( CODEX_SHARED )
  add_library( codex.shared SHARED ${HEADERS} ${SRCS} )
endif()
