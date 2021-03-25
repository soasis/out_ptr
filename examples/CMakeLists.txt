#  Copyright ⓒ 2018-2021 ThePhD.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

# # Dependencies
# Need boost.smart_ptr
# which comes with all these dependencies...
set(BOOST_ENABLE_CMAKE YES)
FetchContent_Declare(
	Boost
	GIT_REPOSITORY https://github.com/boostorg/boost.git
	GIT_SUBMODULES libs/config libs/assert libs/static_assert libs/core 
	               libs/type_traits libs/throw_exception libs/preprocessor
	               libs/smart_ptr libs/static_assert libs/smart_ptr libs/predef libs/move
	               tools/cmake
)
FetchContent_MakeAvailable(Boost)

# Need pthreads
find_package(Threads REQUIRED)

# # Examples
set(ztd_out_ptr_example_sources
	"customization.handle.cpp"
	"customization.traits.handle.cpp"
	"std.custom_unique_ptr.cpp"
	"std.shared_ptr.cpp"
	"std.unique_ptr.cpp"
)

foreach (example_source_name IN LISTS ztd_out_ptr_example_sources)
	set(example_source_file source/${example_source_name})
	set(example_target ztd.out_ptr.${example_source_name})
	add_executable(${example_target} ${example_source_file})
	target_compile_features(${example_target} PRIVATE cxx_std_11)
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

		ztd::out_ptr

		ficapi
		
		${CMAKE_DL_LIBS}
	)
	target_include_directories(${example_target} PRIVATE
		"../vendor/handle/include"
		"include"
	)
	if (ZTD_OUT_PTR_TESTS)
		add_test(NAME ${example_target} COMMAND ${example_target})
	endif()
endforeach()

if (CMAKE_USE_PTHREADS_INIT)
	set(example_source_name "pthread.cpp")
	set(example_source_file source/${example_source_name})
	set(example_target ztd.out_ptr.${example_source_name})
	add_executable(${example_target} ${example_source_file})
	target_compile_features(${example_target} PRIVATE cxx_std_11)
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

		ztd::out_ptr 

		${CMAKE_DL_LIBS}
	)
	target_include_directories(${example_target} PRIVATE
		"include"
	)	
	if (ZTD_OUT_PTR_TESTS)
		add_test(NAME ${example_target} COMMAND ${example_target})
	endif()
endif()


#[[
set(ztd_out_ptr_boost_example_sources
	"com.intrusive_ptr.cpp"
)

foreach (example_source_name IN LISTS ztd_out_ptr_boost_example_sources)
	set(example_source_file source/${example_source_name})
	set(example_target ztd.out_ptr.${example_source_name})
	add_executable(${example_target} ${example_source_file})
	target_compile_features(${example_target} PRIVATE cxx_std_11)
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

		ztd::out_ptr

		Boost::smart_ptr

		ficapi
		
		${CMAKE_DL_LIBS}
	)
	target_include_directories(${example_target} PRIVATE
		"include"
	)
	if (ZTD_OUT_PTR_TESTS)
		add_test(NAME ${example_target} COMMAND ${example_target})
	endif()
endforeach()
]]
