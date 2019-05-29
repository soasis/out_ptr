#  Copyright ⓒ 2018-2019 ThePhD.
#
#  Distributed under the Boost Software License, Version 1.0. (See
#  accompanying file LICENSE or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
#  See http://www.boost.org/libs/out_ptr/ for documentation.

cmake_minimum_required(VERSION 3.5)
project(boost.out_ptr VERSION 1.0.0 DESCRIPTION "A library for having output pointers in C-like functions work well with C++ smart pointer types." LANGUAGES C CXX)

# # Inclusion Work
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# # Options
option(BOOST_OUT_PTR_CI "Whether or not we are in continguous integration environment" OFF)
option(BOOST_OUT_PTR_TESTS "Enable build of tests" OFF)
option(BOOST_OUT_PTR_EXAMPLES "Enable build of examples" OFF)
option(BOOST_OUT_PTR_BENCHMARKS "Enable build of benchmarks" OFF)


# # Targets
add_library(boost_out_ptr INTERFACE)
add_library(Boost::out_ptr ALIAS boost_out_ptr)

target_include_directories(boost_out_ptr INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
	$<INSTALL_INTERFACE:include>
)

# # Config / Version packaging
# Version configurations
configure_package_config_file(
	cmake/boost_out_ptr-config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/boost/out_ptr-config.cmake"
	INSTALL_DESTINATION lib/cmake/boost/out_ptr
	NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/boost/out_ptr-config-version.cmake"
	COMPATIBILITY AnyNewerVersion)

export(TARGETS boost_out_ptr FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/boost/out_ptr-targets.cmake")

set(boost_out_ptr_sources 
	"include/boost/out_ptr.hpp"
	"include/boost/out_ptr/out_ptr.hpp"
	"include/boost/out_ptr/inout_ptr.hpp"
	"include/boost/out_ptr/simple_out_ptr.hpp"
	"include/boost/out_ptr/simple_inout_ptr.hpp"
	"include/boost/out_ptr/clever_out_ptr.hpp"
	"include/boost/out_ptr/clever_inout_ptr.hpp"
)

#target_sources(boost_out_ptr INTERFACE 
#	$<BUILD_INTERFACE:${boost_out_ptr_sources}>  
#	$<INSTALL_INTERFACE:${boost_out_ptr_sources}>
#)
set_target_properties(boost_out_ptr
	PROPERTIES
	EXPORT_NAME Boost::out_ptr
)

#install(TARGETS boost_out_ptr
#	EXPORT boost_out_ptr)

#install(EXPORT boost_out_ptr
#	FILE boost_out_ptr-targets.cmake
#	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/boost/out_ptr")

#install(DIRECTORY "include/boost/out_ptr"
#	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

#install(FILES
#	"${CMAKE_CURRENT_BINARY_DIR}/cmake/boost/out_ptr-config.cmake"
#	"${CMAKE_CURRENT_BINARY_DIR}/cmake/boost/out_ptr-config-version.cmake"
#	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/boost")


# # Benchmarks, Tests, Examples
if (BOOST_OUT_PTR_TESTS OR BOOST_OUT_PTR_EXAMPLES OR BOOST_OUT_PTR_BENCHMARKS)
	if (IS_X86 OR CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/bin")
	else()
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/bin")
	endif()

	if (MSVC)
		string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
		string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	endif()

	enable_testing()
	add_subdirectory(vendor/boost/config)
	add_subdirectory(vendor/boost/core)
	add_subdirectory(vendor/boost/assert)
	add_subdirectory(vendor/boost/mp11)
	# test and benchmark deps
	add_subdirectory(vendor/ficapi)
endif()

if (BOOST_OUT_PTR_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif(BOOST_OUT_PTR_TESTS)
if (BOOST_OUT_PTR_BENCHMARKS)
	add_subdirectory(benchmarks)
endif(BOOST_OUT_PTR_BENCHMARKS)
if (BOOST_OUT_PTR_EXAMPLES)
	add_subdirectory(examples)
endif(BOOST_OUT_PTR_EXAMPLES)
