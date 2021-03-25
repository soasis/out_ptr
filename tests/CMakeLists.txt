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

# Test dependency: catch2
include(FetchContent)

FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG        v2.13.4
)
FetchContent_MakeAvailable(Catch2)

file(GLOB_RECURSE ztd.out_ptr.tests_sources
	LIST_DIRECTORIES FALSE
	source/*.*
)

add_executable(ztd.out_ptr.tests ${ztd.out_ptr.tests_sources})
target_compile_features(ztd.out_ptr.tests PRIVATE cxx_std_11)
target_compile_definitions(ztd.out_ptr.tests
	PRIVATE ZTD_OUT_PTR_CLEVER_SANITY_CHECK=1)
if (MSVC)
	target_compile_options(ztd.out_ptr.tests
		PRIVATE /W4)
else()
	target_compile_options(ztd.out_ptr.tests
		PRIVATE -Wpedantic -Wall -Werror)
endif()
target_include_directories(ztd.out_ptr.tests
	PRIVATE 
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"../vendor/handle/include"
)
target_link_libraries(ztd.out_ptr.tests 
	PRIVATE 

	ztd::out_ptr 
	
	Catch2::Catch2 
	
	ficapi 
	
	${CMAKE_DL_LIBS}
)
add_test(NAME ztd.out_ptr.tests COMMAND ztd.out_ptr.tests)

# # failure tests
add_subdirectory(failure)
