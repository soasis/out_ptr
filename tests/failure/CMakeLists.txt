#  Copyright ⓒ 2018-2019 ThePhD.
#
#  Distributed under the Boost Software License, Version 1.0. (See
#  accompanying file LICENSE or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
#  See http://www.boost.org/libs/out_ptr/ for documentation.

set(boost_out_ptr_failure_tests_sources
	"inout_ptr.shared_ptr.cpp"
	"inout_ptr.shared_ptr.deleter.cpp"
	"out_ptr.shared_ptr.deleter.cpp"
)

foreach (test_name ${boost_out_ptr_failure_tests_sources})
	set(test_file source/${test_name})
	set(test_target boost.out_ptr.fail.${test_name})
	add_executable(${test_target} ${test_file} "source/main.cpp")
	if (MSVC)
	else()
		target_compile_options(${test_target} 
			PRIVATE -std=c++11)
	endif()
	set_target_properties(${test_target} 
		PROPERTIES 
		EXCLUDE_FROM_ALL TRUE
		EXCLUDE_FROM_DEFAULT_BUILD TRUE
	)
	target_include_directories(${test_target}
		PRIVATE 
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
		"../vendor/handle/include"
	)
	target_link_libraries(${test_target}
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
	add_test(NAME ${test_target} COMMAND cmake --build . --target ${test_target}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	set_tests_properties(${test_target} 
		PROPERTIES 
		WILL_FAIL true)
endforeach()
