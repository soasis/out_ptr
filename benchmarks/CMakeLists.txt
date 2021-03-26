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

include(FetchContent)

# Get Python interpreter for cache stuff
find_package(Python3)


set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
FetchContent_Declare(
	googlebenchmark
	GIT_REPOSITORY https://github.com/google/benchmark.git
)
FetchContent_MakeAvailable(
	googlebenchmark
)


file(GLOB_RECURSE ztd_out_ptr_benchmarks_sources
	LIST_DIRECTORIES FALSE
	CONFIGURE_DEPENDS
	source/*.*
)

add_executable(ztd.out_ptr.benchmarks ${ztd_out_ptr_benchmarks_sources})
target_include_directories(ztd.out_ptr.benchmarks PRIVATE
	"include")
if (MSVC)
	target_compile_options(ztd.out_ptr.benchmarks
		PRIVATE /W4)
else()
	target_compile_options(ztd.out_ptr.benchmarks
		PRIVATE -Wpedantic -Wall -Werror)
endif()
target_link_libraries(ztd.out_ptr.benchmarks
	PRIVATE 
	
	ztd::out_ptr 
	
	ficapi 
	
	benchmark::benchmark 
	
	${CMAKE_DL_LIBS}
)

set(ZTD_OUT_PTR_BENCHMARKS_REPETITIONS 150
	CACHE STRING "The number of times to re-rerun the benchmarks to gather additional data")
set(ZTD_OUT_PTR_BENCHMARKS_FORMAT json 
	CACHE STRING "The output format of the data. Must be json")
string(TOLOWER ${ZTD_OUT_PTR_BENCHMARKS_FORMAT} ZTD_OUT_PTR_BENCHMARKS_FORMAT_LOWER)
set(ZTD_OUT_PTR_BENCHMARKS_FORMAT ${ZTD_OUT_PTR_BENCHMARKS_FORMAT_LOWER} 
	CACHE STRING "The output format of the data. Must be json" FORCE)

set(ZTD_OUT_PTR_BENCHMARKS_RESULTS_OUTDIR "${CMAKE_SOURCE_DIR}/benchmark_results")
set(ZTD_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE "${ZTD_OUT_PTR_BENCHMARKS_RESULTS_OUTDIR}/out_ptr_benchmarks.${ZTD_OUT_PTR_BENCHMARKS_FORMAT}")
set(ZTD_OUT_PTR_BENCHMARKS_GRAPH_OUTDIR "${CMAKE_SOURCE_DIR}/benchmark_results")
file(MAKE_DIRECTORY "${ZTD_OUT_PTR_BENCHMARKS_RESULTS_OUTDIR}")

set(ztd_out_ptr_benchmarks_categories shared_local_out_ptr shared_reset_out_ptr local_out_ptr reset_out_ptr local_inout_ptr reset_inout_ptr)

add_custom_command(
	OUTPUT "${ZTD_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}"
	COMMAND ztd.out_ptr.benchmarks 
		"--benchmark_out=${ZTD_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}" 
		"--benchmark_out_format=${ZTD_OUT_PTR_BENCHMARKS_FORMAT}" 
		"--benchmark_repetitions=${ZTD_OUT_PTR_BENCHMARKS_REPETITIONS}"
	DEPENDS ztd.out_ptr.benchmarks
	COMMENT "Executing Benchmarks and outputting to '${ZTD_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}'"
)

if (Python3_Interpreter_FOUND)
	add_custom_target(ztd.out_ptr.benchmarks_graphs_generator
		COMMAND ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/benchmarks/tools/generate_graphs.py" 
			"--input=${ZTD_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}"
			"--input_format=${ZTD_OUT_PTR_BENCHMARKS_FORMAT}"
			"--output_dir=${ZTD_OUT_PTR_BENCHMARKS_GRAPH_OUTDIR}"
			--categories ${ztd_out_ptr_benchmarks_categories}
		DEPENDS ztd.out_ptr.benchmarks "${ZTD_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}"
		COMMENT "Graphing '${ZTD_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}' data to '${ZTD_OUT_PTR_BENCHMARKS_RESULTS_OUTDIR}'"
	)
endif()
