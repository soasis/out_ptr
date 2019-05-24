#  Copyright ⓒ 2018-2019 ThePhD.
#
#  Distributed under the Boost Software License, Version 1.0. (See
#  accompanying file LICENSE or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
#  See http://www.boost.org/libs/out_ptr/ for documentation.

# Test dependency: catch2
add_subdirectory(../vendor/catch2 "vendor/catch2")

set(boost.out_ptr.tests_sources
	"source/clever_out_ptr_test.cpp"
	"source/simple_out_ptr_test.cpp"
	"source/out_ptr_test.cpp"
	"source/raw_out_ptr_test.cpp"
	"source/customized_out_ptr_test.cpp"
	"source/custom_unique_deleter_test.cpp"

	"source/exceptions_inout_ptr_test.cpp"
	"source/exceptions_out_ptr_test.cpp"

	"source/clever_inout_ptr_test.cpp"
	"source/simple_inout_ptr_test.cpp"
	"source/inout_ptr_test.cpp"
	"source/customized_inout_ptr_test.cpp"

	"source/footguns.cpp"

	"source/out_ptr_tests_main.cpp"
)

add_executable(boost.out_ptr.tests ${boost.out_ptr.tests_sources})
if (MSVC)
	target_compile_options(${example_target}
		PRIVATE /W4)
else()
	target_compile_options(boost.out_ptr.tests
		PRIVATE -std=c++11 -Wpedantic -Wall -Werror)
endif()
target_include_directories(boost.out_ptr.tests
	PRIVATE 
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"../vendor/handle/include"
)
target_link_libraries(boost.out_ptr.tests 
	PRIVATE 
	Boost::assert
	Boost::config
	Boost::core
	Boost::mp11
	Boost::out_ptr 
	
	Catch2::Catch2 
	
	ficapi 
	
	${CMAKE_DL_LIBS}
)
add_test(NAME boost.out_ptr.tests COMMAND boost.out_ptr.tests)

# # failure tests
add_subdirectory(failure)
