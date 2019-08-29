#  Copyright ⓒ 2018-2019 ThePhD.
#
#  Distributed under the Boost Software License, Version 1.0. (See
#  accompanying file LICENSE or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
#  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

# Get Python interpreter for cache stuff
find_package(Python3)

# Subdirectory Google Benchmark, after setting all cache variables
set(BENCHMARK_ENABLE_EXCEPTIONS ON CACHE INTERNAL "Enable the use of exceptions in the benchmark library." FORCE)
set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "Enable testing of the benchmark library." FORCE)
set(BENCHMARK_ENABLE_LTO OFF CACHE INTERNAL "Enable link time optimisation of the benchmark library." FORCE)
set(BENCHMARK_USE_LIBCXX OFF CACHE INTERNAL "Build and test using libc++ as the standard library." FORCE)
set(BENCHMARK_BUILD_32_BITS OFF CACHE INTERNAL "Build a 32 bit version of the library." FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE INTERNAL "Enable installation of benchmark. (Projects embedding benchmark may want to turn this OFF.)" FORCE)
set(BENCHMARK_DOWNLOAD_DEPENDENCIES OFF CACHE INTERNAL "Allow the downloading and in-tree building of unmet dependencies" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE INTERNAL "Enable building the unit tests which depend on gtest" FORCE)
add_subdirectory("../vendor/benchmark" "vendor/benchmark")

set(phd_out_ptr_benchmarks_sources
	"include/benchmarks/statistics.hpp"
	"source/statistics.cpp"

	"source/shared_local_out_ptr.cpp"
	"source/shared_reset_out_ptr.cpp"

	"source/local_out_ptr.cpp"
	"source/reset_out_ptr.cpp"

	"source/local_inout_ptr.cpp"
	"source/reset_inout_ptr.cpp"

	"source/out_ptr_benchmarks_main.cpp"
)

add_executable(phd.out_ptr.benchmarks ${phd_out_ptr_benchmarks_sources})
target_include_directories(phd.out_ptr.benchmarks PRIVATE
	"include")
if (MSVC)
	target_compile_options(phd.out_ptr.benchmarks
		PRIVATE /W4)
else()
	target_compile_options(phd.out_ptr.benchmarks
		PRIVATE -std=c++11 -Wpedantic -Wall -Werror)
endif()
target_link_libraries(phd.out_ptr.benchmarks
	PRIVATE 
	
	phd::out_ptr 
	
	ficapi 
	
	benchmark 
	
	${CMAKE_DL_LIBS}
)

set(PHD_OUT_PTR_BENCHMARKS_REPETITIONS 150
	CACHE STRING "The number of times to re-rerun the benchmarks to gather additional data")
set(PHD_OUT_PTR_BENCHMARKS_FORMAT json 
	CACHE STRING "The output format of the data. Must be json")
string(TOLOWER ${PHD_OUT_PTR_BENCHMARKS_FORMAT} PHD_OUT_PTR_BENCHMARKS_FORMAT_LOWER)
set(PHD_OUT_PTR_BENCHMARKS_FORMAT ${PHD_OUT_PTR_BENCHMARKS_FORMAT_LOWER} 
	CACHE STRING "The output format of the data. Must be json" FORCE)

set(PHD_OUT_PTR_BENCHMARKS_RESULTS_OUTDIR "${CMAKE_SOURCE_DIR}/benchmark_results")
set(PHD_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE "${PHD_OUT_PTR_BENCHMARKS_RESULTS_OUTDIR}/out_ptr_benchmarks.${PHD_OUT_PTR_BENCHMARKS_FORMAT}")
set(PHD_OUT_PTR_BENCHMARKS_GRAPH_OUTDIR "${CMAKE_SOURCE_DIR}/benchmark_results")
file(MAKE_DIRECTORY "${PHD_OUT_PTR_BENCHMARKS_RESULTS_OUTDIR}")

set(phd_out_ptr_benchmarks_categories shared_local_out_ptr shared_reset_out_ptr local_out_ptr reset_out_ptr local_inout_ptr reset_inout_ptr)

add_custom_command(
	OUTPUT "${PHD_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}"
	COMMAND phd.out_ptr.benchmarks 
		"--benchmark_out=${PHD_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}" 
		"--benchmark_out_format=${PHD_OUT_PTR_BENCHMARKS_FORMAT}" 
		"--benchmark_repetitions=${PHD_OUT_PTR_BENCHMARKS_REPETITIONS}"
	DEPENDS phd.out_ptr.benchmarks
	COMMENT "Executing Benchmarks and outputting to '${PHD_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}'"
)

if (Python3_Interpreter_FOUND)
	add_custom_target(phd.out_ptr.benchmarks_graphs_generator
		COMMAND ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/benchmarks/tools/generate_graphs.py" 
			"--input=${PHD_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}"
			"--input_format=${PHD_OUT_PTR_BENCHMARKS_FORMAT}"
			"--output_dir=${PHD_OUT_PTR_BENCHMARKS_GRAPH_OUTDIR}"
			--categories ${phd_out_ptr_benchmarks_categories}
		DEPENDS phd.out_ptr.benchmarks "${PHD_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}"
		COMMENT "Graphing '${PHD_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}' data to '${PHD_OUT_PTR_BENCHMARKS_RESULTS_OUTDIR}'"
	)
endif()
