#  Copyright ⓒ 2018-2019 ThePhD.
#
#  Distributed under the Boost Software License, Version 1.0. (See
#  accompanying file LICENSE or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
#  See http://www.boost.org/libs/out_ptr/ for documentation.

# Test dependency: catch2
add_subdirectory(../vendor/catch2 "vendor/catch2")

set(phd.out_ptr.tests_sources
	"source/clever_out_ptr_test.cpp"
	"source/simple_out_ptr_test.cpp"
	"source/out_ptr_test.cpp"
	"source/raw_out_ptr_test.cpp"
	"source/exceptions_out_ptr_test.cpp"
	"source/customized_out_ptr_test.cpp"
	"source/customized_traits_out_ptr_test.cpp"
	"source/custom_unique_deleter_test.cpp"

	"source/clever_inout_ptr_test.cpp"
	"source/simple_inout_ptr_test.cpp"
	"source/inout_ptr_test.cpp"
	"source/raw_inout_ptr_test.cpp"
	"source/exceptions_inout_ptr_test.cpp"
	"source/customized_inout_ptr_test.cpp"
	"source/customized_traits_inout_ptr_test.cpp"

	"source/footguns.cpp"
	"source/const_test.cpp"

	"source/main.cpp"
)

add_executable(phd.out_ptr.tests ${phd.out_ptr.tests_sources})
set_target_properties(${example_target} PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED TRUE
	CXX_EXTENSIONS FALSE
)
target_compile_definitions(phd.out_ptr.tests
	PRIVATE PHD_OUT_PTR_CLEVER_SANITY_CHECK=1)
if (MSVC)
	target_compile_options(phd.out_ptr.tests
		PRIVATE /W4)
else()
	target_compile_options(phd.out_ptr.tests
		PRIVATE -Wpedantic -Wall -Werror)
endif()
target_include_directories(phd.out_ptr.tests
	PRIVATE 
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"../vendor/handle/include"
)
target_link_libraries(phd.out_ptr.tests 
	PRIVATE 

	phd::out_ptr 
	
	Catch2::Catch2 
	
	ficapi 
	
	${CMAKE_DL_LIBS}
)
add_test(NAME phd.out_ptr.tests COMMAND phd.out_ptr.tests)

# # failure tests
add_subdirectory(failure)
