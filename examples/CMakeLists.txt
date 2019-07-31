#  Copyright ⓒ 2018-2019 ThePhD.
#
#  Distributed under the Boost Software License, Version 1.0. (See
#  accompanying file LICENSE or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
#  See http://www.boost.org/libs/out_ptr/ for documentation.

# # Dependencies
# Need boost.smart_ptr
# which comes with all these dependencies...
add_subdirectory(../vendor/boost/move "vendor/boost/move")
add_subdirectory(../vendor/boost/predef "vendor/boost/predef")
add_subdirectory(../vendor/boost/static_assert "vendor/boost/static_assert")
add_subdirectory(../vendor/boost/throw_exception "vendor/boost/throw_exception")
add_subdirectory(../vendor/boost/type_traits "vendor/boost/type_traits")
add_subdirectory(../vendor/boost/smart_ptr "vendor/boost/smart_ptr")

# Need pthreads
find_package(Threads REQUIRED)

# # Examples
set(phd_out_ptr_example_sources
	"customization.handle.cpp"
	"customization.traits.handle.cpp"
	"std.custom_unique_ptr.cpp"
	"std.shared_ptr.cpp"
	"std.unique_ptr.cpp"
)

foreach (example_source_name ${phd_out_ptr_example_sources})
	set(example_source_file source/${example_source_name})
	set(example_target phd.out_ptr.${example_source_name})
	add_executable(${example_target} ${example_source_file})
	set_target_properties(${example_target} PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED TRUE
		CXX_EXTENSIONS FALSE
	)
	if (MSVC)
		target_compile_options(${example_target}
			PRIVATE /W4)
	else()
		target_compile_options(${example_target}
			PRIVATE -Wpedantic -Wall -Werror)
	endif()
	target_compile_definitions(${example_target} PRIVATE __STDC_WANT_LIB_EXT1__=1)
	target_link_libraries(${example_target}
		PRIVATE 
		
		Threads::Threads

		phd::out_ptr

		Boost::config
		Boost::core
		Boost::mp11
		Boost::smart_ptr

		ficapi
		
		${CMAKE_DL_LIBS}
	)
	target_include_directories(${example_target} PRIVATE
		"../vendor/handle/include"
		"include"
	)
	if (PHD_OUT_PTR_TESTS)
		add_test(NAME ${example_target} COMMAND ${example_target})
	endif()
endforeach()

set(phd_out_ptr_atl_dir "${CMAKE_BINARY_DIR}/phd.out_ptr.test_atl_include")
set(phd_out_ptr_atl_bin_dir "${phd_out_ptr_atl_dir}/bin")
set(phd_out_ptr_atl_file "${phd_out_ptr_atl_dir}/atl_test.cpp")
file(MAKE_DIRECTORY ${phd_out_ptr_atl_dir})
file(MAKE_DIRECTORY ${phd_out_ptr_atl_bin_dir})
file(WRITE ${phd_out_ptr_atl_file}
"#include <atlbase.h>
int main (int, char*[]) { return 0; }")
try_run(phd_out_ptr_examples_has_atl_run phd_out_ptr_examples_has_atl_compile 
	${phd_out_ptr_atl_bin_dir} ${phd_out_ptr_atl_file}
	CMAKE_FLAGS "CMAKE_CXX_FLAGS:${CMAKE_CXX_FLAGS}" "CMAKE_C_FLAGS:${CMAKE_C_FLAGS}"
	"CMAKE_C_COMPILER:${CMAKE_C_COMPILER}" "CMAKE_CXX_COMPILER:${CMAKE_CXX_COMPILER}"
)
if (phd_out_ptr_examples_has_atl_run)
	set(example_source_name "com.intrusive_ptr.cpp")
	set(example_source_file source/${example_source_name})
	set(example_target phd.out_ptr.${example_source_name})
	add_executable(${example_target} ${example_source_file})
	set_target_properties(${example_target} PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED TRUE
		CXX_EXTENSIONS FALSE
	)
	if (MSVC)
		target_compile_options(${example_target}
			PRIVATE /W4)
	else()
		target_compile_options(${example_target}
			PRIVATE -Wpedantic -Wall -Werror)
	endif()
	target_compile_definitions(${example_target} PRIVATE __STDC_WANT_LIB_EXT1__=1)
	target_link_libraries(${example_target}
		PRIVATE 
		
		Threads::Threads

		phd::out_ptr 

		Boost::config
		Boost::core
		Boost::mp11
		Boost::smart_ptr
		
		${CMAKE_DL_LIBS}
	)
	target_include_directories(${example_target} PRIVATE
		"include"
	)
	if (PHD_OUT_PTR_TESTS)
		add_test(NAME ${example_target} COMMAND ${example_target})
	endif()
endif()

if (CMAKE_USE_PTHREADS_INIT)
	set(example_source_name "pthread.cpp")
	set(example_source_file source/${example_source_name})
	set(example_target phd.out_ptr.${example_source_name})
	add_executable(${example_target} ${example_source_file})
	set_target_properties(${example_target} PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED TRUE
		CXX_EXTENSIONS FALSE
	)
	if (MSVC)
		target_compile_options(${example_target}
			PRIVATE /W4)
	else()
		target_compile_options(${example_target}
			PRIVATE -Wpedantic -Wall -Werror)
	endif()
	target_compile_definitions(${example_target} PRIVATE __STDC_WANT_LIB_EXT1__=1)
	target_link_libraries(${example_target}
		PRIVATE 
		
		Threads::Threads

		phd::out_ptr 

		Boost::config
		Boost::core
		Boost::mp11
		Boost::smart_ptr
		
		${CMAKE_DL_LIBS}
	)
	target_include_directories(${example_target} PRIVATE
		"include"
	)	
	if (PHD_OUT_PTR_TESTS)
		add_test(NAME ${example_target} COMMAND ${example_target})
	endif()
endif()
