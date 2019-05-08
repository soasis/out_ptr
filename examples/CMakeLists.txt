#  Copyright ⓒ 2018-2019 ThePhD.
#
#  Distributed under the Boost Software License, Version 1.0. (See
#  accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
#  See http://www.boost.org/libs/out_ptr/ for documentation.

set(boost_out_ptr_example_sources
	"boost.shared_ptr.cpp"
	"com.intrusive_ptr.cpp"
	"custom.handle.cpp"
	"std.custom_unique_ptr.cpp"
	"std.shared_ptr.cpp"
	"std.unique_ptr.cpp"
)

foreach (example_source_name ${boost_out_ptr_example_sources})
	set(example_source_file source/${example_source_name})
	set(example_target boost.out_ptr.${example_source_name})
	add_executable(${example_target} ${example_source_file})
	if (MSVC)
	else()
		target_compile_options(${example_target}
			PRIVATE -std=c++11)
	endif()
	target_compile_definitions(${example_target} PRIVATE __STDC_WANT_LIB_EXT1__=1)
	target_link_libraries(${example_target}
		PRIVATE 
		
		Boost::assert
		Boost::config
		Boost::core
		Boost::mp11
		Boost::out_ptr 
		
		ficapi
		
		${CMAKE_DL_LIBS}
	)
	target_include_directories(${example_target} PRIVATE
		"../vendor/handle/include"
	)
	if (BOOST_OUT_PTR_TESTS)
		add_test(NAME ${example_target} COMMAND ${example_target})
	endif()
endforeach()
