#  Copyright ⓒ 2018-2019 ThePhD.
#
#  Distributed under the Boost Software License, Version 1.0. (See
#  accompanying file LICENSE or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
#  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

cmake_minimum_required(VERSION 3.5)
project(phd.out_ptr VERSION 1.0.0 DESCRIPTION "A library for having output pointers in C-like functions work well with C++ smart pointer types." LANGUAGES C CXX)

# # Inclusion Work
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# # Top Level Directories
# Check if this is the top-level project or not
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(PHD_OUT_PTR_IS_TOP_LEVEL_PROJECT true)
else()
    set(PHD_OUT_PTR_IS_TOP_LEVEL_PROJECT false)
endif()

# Modify bad flags / change defaults if that is the case
if (PHD_OUT_PTR_IS_TOP_LEVEL_PROJECT)
	if (CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CFG_INT_DIR}/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CFG_INT_DIR}/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CFG_INT_DIR}/bin")
	else()
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CFG_INT_DIR}/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CFG_INT_DIR}/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CFG_INT_DIR}/bin")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 4)

	if (MSVC)
		string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
		string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	endif()
endif()

# # Options
option(PHD_OUT_PTR_CI "Whether or not we are in continguous integration environment" OFF)
option(PHD_OUT_PTR_TESTS "Enable build of tests" OFF)
option(PHD_OUT_PTR_EXAMPLES "Enable build of examples" OFF)
option(PHD_OUT_PTR_BENCHMARKS "Enable build of benchmarks" OFF)


# # Targets
add_library(phd_out_ptr INTERFACE)
add_library(phd::out_ptr ALIAS phd_out_ptr)

target_include_directories(phd_out_ptr INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
	$<INSTALL_INTERFACE:include>
)

# # Config / Version packaging
# Version configurations
configure_package_config_file(
	cmake/phd_out_ptr-config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/phd/out_ptr-config.cmake"
	INSTALL_DESTINATION lib/cmake/phd/out_ptr
	NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/phd/out_ptr-config-version.cmake"
	COMPATIBILITY AnyNewerVersion)

export(TARGETS phd_out_ptr FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/phd/out_ptr-targets.cmake")

set(phd_out_ptr_sources 
	"include/phd/out_ptr.hpp"
	"include/phd/out_ptr/out_ptr.hpp"
	"include/phd/out_ptr/inout_ptr.hpp"
	"include/phd/out_ptr/simple_out_ptr.hpp"
	"include/phd/out_ptr/simple_inout_ptr.hpp"
	"include/phd/out_ptr/clever_out_ptr.hpp"
	"include/phd/out_ptr/clever_inout_ptr.hpp"
)

#target_sources(phd_out_ptr INTERFACE 
#	$<BUILD_INTERFACE:${phd_out_ptr_sources}>  
#	$<INSTALL_INTERFACE:${phd_out_ptr_sources}>
#)
set_target_properties(phd_out_ptr
	PROPERTIES
	EXPORT_NAME phd::out_ptr
)

#install(TARGETS phd_out_ptr
#	EXPORT phd_out_ptr)

#install(EXPORT phd_out_ptr
#	FILE phd_out_ptr-targets.cmake
#	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/phd/out_ptr")

#install(DIRECTORY "include/phd/out_ptr"
#	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

#install(FILES
#	"${CMAKE_CURRENT_BINARY_DIR}/cmake/phd/out_ptr-config.cmake"
#	"${CMAKE_CURRENT_BINARY_DIR}/cmake/phd/out_ptr-config-version.cmake"
#	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/boost")

if (PHD_OUT_PTR_BENCHMARKS OR PHD_OUT_PTR_TESTS OR PHD_OUT_PTR_EXAMPLES)
	# test and benchmark deps
	add_subdirectory(vendor/boost/config)
	add_subdirectory(vendor/boost/core)
	add_subdirectory(vendor/boost/assert)
	add_subdirectory(vendor/ficapi)
endif()

if (PHD_OUT_PTR_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif(PHD_OUT_PTR_TESTS)
if (PHD_OUT_PTR_BENCHMARKS)
	add_subdirectory(benchmarks)
endif(PHD_OUT_PTR_BENCHMARKS)
if (PHD_OUT_PTR_EXAMPLES)
	add_subdirectory(examples)
endif(PHD_OUT_PTR_EXAMPLES)
