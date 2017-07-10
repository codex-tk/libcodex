CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

project( libcodex )

#option( MyOption "MyOption" OFF )
#cmake -DMyOption=ON project -D...

option( LIBCODEX_SHARED "Shared Build Option" OFF )

if(MSVC)
	add_compile_options("/wd4819")
	add_compile_options("/wd4127")
	foreach (flag_var
				 CMAKE_CXX_FLAGS
				 CMAKE_CXX_FLAGS_DEBUG
				 CMAKE_CXX_FLAGS_RELEASE
				 CMAKE_CXX_FLAGS_MINSIZEREL
				 CMAKE_CXX_FLAGS_RELWITHDEBINFO)
			string(REPLACE "/MD" "-MT" ${flag_var} "${${flag_var}}")
			string(REPLACE "/W3" "/W4" ${flag_var} "${${flag_var}}")
	endforeach()

	string(REGEX REPLACE "/O2" "Od"
			CMAKE_CXX_FLAGS_DEBUG
			"${CMAKE_CXX_FLAGS_DEBUG}")
else()
	add_definitions( -Wall -std=c++1z -o2 -g -ggdb )
endif()

add_subdirectory( googletest )
add_subdirectory( codex )
add_subdirectory( tests )
