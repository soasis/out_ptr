#  Copyright ⓒ 2018-2019 ThePhD.
#
#  Distributed under the Boost Software License, Version 1.0. (See
#  accompanying file LICENSE or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
#  See http://www.boost.org/libs/out_ptr/ for documentation.

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

set(boost_out_ptr_benchmarks_sources
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

add_executable(boost.out_ptr.benchmarks ${boost_out_ptr_benchmarks_sources})
target_include_directories(boost.out_ptr.benchmarks PRIVATE
	"include")
if (MSVC)
	target_compile_options(boost.out_ptr.benchmarks
		PRIVATE /W4)
else()
	target_compile_options(boost.out_ptr.benchmarks
		PRIVATE -std=c++11 -Wpedantic -Wall -Werror)
endif()
target_link_libraries(boost.out_ptr.benchmarks
	PRIVATE 
	
	Boost::config
	Boost::core
	Boost::assert
	Boost::mp11
	Boost::out_ptr 
	
	ficapi 
	
	benchmark 
	
	${CMAKE_DL_LIBS}
)

set(BOOST_OUT_PTR_BENCHMARKS_REPETITIONS 250
	CACHE STRING "The number of times to re-rerun the benchmarks to gather additional data")
set(BOOST_OUT_PTR_BENCHMARKS_FORMAT json 
	CACHE STRING "The output format of the data. Must be json or csv")
string(TOLOWER ${BOOST_OUT_PTR_BENCHMARKS_FORMAT} BOOST_OUT_PTR_BENCHMARKS_FORMAT_LOWER)
set(BOOST_OUT_PTR_BENCHMARKS_FORMAT ${BOOST_OUT_PTR_BENCHMARKS_FORMAT_LOWER} 
	CACHE STRING "The output format of the data. Must be json or csv" FORCE)

set(BOOST_OUT_PTR_BENCHMARKS_RESULTS_OUTDIR "${CMAKE_SOURCE_DIR}/benchmark_results/out_ptr")
set(BOOST_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE "${BOOST_OUT_PTR_BENCHMARKS_RESULTS_OUTDIR}/out_ptr_benchmarks.${BOOST_OUT_PTR_BENCHMARKS_FORMAT}")
set(BOOST_OUT_PTR_BENCHMARKS_GRAPH_OUTDIR "${CMAKE_SOURCE_DIR}/benchmark_results/out_ptr")
file(MAKE_DIRECTORY "${BOOST_OUT_PTR_BENCHMARKS_RESULTS_OUTDIR}")

set(boost_out_ptr_benchmarks_categories shared_local_out_ptr shared_reset_out_ptr local_out_ptr reset_out_ptr local_inout_ptr reset_inout_ptr)

add_custom_command(
	OUTPUT "${BOOST_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}"
	COMMAND boost.out_ptr.benchmarks 
		"--benchmark_out=${BOOST_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}" 
		"--benchmark_out_format=${BOOST_OUT_PTR_BENCHMARKS_FORMAT}" 
		"--benchmark_repetitions=${BOOST_OUT_PTR_BENCHMARKS_REPETITIONS}"
	DEPENDS boost.out_ptr.benchmarks
	COMMENT "Executing Benchmarks and outputting to '${BOOST_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}'"
)

if (Python3_Interpreter_FOUND)
	add_custom_target(boost.out_ptr.benchmarks_graphs_generator
		COMMAND ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/benchmarks/tools/generate_graphs.py" 
			"--input=${BOOST_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}"
			"--input_format=${BOOST_OUT_PTR_BENCHMARKS_FORMAT}"
			"--output_dir=${BOOST_OUT_PTR_BENCHMARKS_GRAPH_OUTDIR}"
			--categories ${boost_out_ptr_benchmarks_categories}
		DEPENDS boost.out_ptr.benchmarks "${BOOST_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}"
		COMMENT "Graphing '${BOOST_OUT_PTR_BENCHMARKS_RESULTS_OUTFILE}' data to '${BOOST_OUT_PTR_BENCHMARKS_RESULTS_OUTDIR}'"
	)
endif()
