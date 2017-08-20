project( codex.tests )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../codex/include )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../googletest/googletest/include )

link_directories( ${CMAKE_CURRENT_BINARY_DIR}/../codex )

if( MSVC )
	set(BOOST_ROOT "C:/Users/codex/works/boost_1_64_0")
	add_definitions (/D _CRT_SECURE_NO_WARNINGS)
else()
	add_definitions( -Wall -std=c++11 -o2 -g -ggdb )
	if( APPLE )

	else()

	endif()
endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)

find_package (Boost 1.60.0 REQUIRED COMPONENTS system date_time regex )

if (Boost_FOUND)
  include_directories (SYSTEM ${Boost_INCLUDE_DIR})
endif()

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
  set( PLATFORM_DEPEND_HEADERS
    ${PLATFORM_DEPEND_HEADERS}
    )
  set( PLATFORM_DEPEND_SRCS
    ${PLATFORM_DEPEND_SRCS}
    )
endif()

add_executable( codex.tests
  test_hello.cpp
  test_function.cpp
  test_container_of.cpp
  test_error.cpp
  test_conv.cpp
  test_main.cpp
  test_boost_regex.cpp
	test_singleton_dclp.cpp
	test_test.cpp
  ${PLATFORM_DEPEND_SRCS}
)

target_link_libraries( codex.tests
 codex
 gtest
 ${Boost_LIBRARIES}
 )
