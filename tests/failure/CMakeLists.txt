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

set(ztd_out_ptr_failure_tests_sources
	"inout_ptr.shared_ptr.cpp"
	"inout_ptr.shared_ptr.deleter.cpp"
	"out_ptr.shared_ptr.deleter.cpp"
)

foreach (test_name ${ztd_out_ptr_failure_tests_sources})
	set(test_file source/${test_name})
	set(test_target ztd.out_ptr.fail.${test_name})
	add_executable(${test_target} ${test_file} "source/main.cpp")
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
		
		ztd::out_ptr 
		
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
