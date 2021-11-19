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

cmake_minimum_required(VERSION 3.15)
project(ztd.out_ptr VERSION 1.0.0 DESCRIPTION "A library for having output pointers in C-like functions work well with C++ smart pointer types." LANGUAGES C CXX)

# # Inclusion Work
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(FetchContent)

# # Top Level Directories
# Check if this is the top-level project or not
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(ZTD_OUT_PTR_IS_TOP_LEVEL_PROJECT true)
else()
    set(ZTD_OUT_PTR_IS_TOP_LEVEL_PROJECT false)
endif()

# Modify bad flags / change defaults if that is the case
if (ZTD_OUT_PTR_IS_TOP_LEVEL_PROJECT)
	if (CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/bin")
	else()
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/bin")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 4)

	set(CMAKE_CXX_STANDARD_REQUIRED YES)
	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_EXTENSIONS NO)
endif()

# # Options
option(ZTD_OUT_PTR_CI "Whether or not we are in continguous integration environment" OFF)
option(ZTD_OUT_PTR_TESTS "Enable build of tests" OFF)
option(ZTD_OUT_PTR_EXAMPLES "Enable build of examples" OFF)
option(ZTD_OUT_PTR_BENCHMARKS "Enable build of benchmarks" OFF)


# # Targets
add_library(ztd_out_ptr INTERFACE)
add_library(ztd::out_ptr ALIAS ztd_out_ptr)

target_include_directories(ztd_out_ptr INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
	$<INSTALL_INTERFACE:include>
)

# # Config / Version packaging
# Version configurations
configure_package_config_file(
	cmake/ztd_out_ptr-config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/ztd/out_ptr-config.cmake"
	INSTALL_DESTINATION lib/cmake/ztd/out_ptr
	NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/ztd/out_ptr-config-version.cmake"
	COMPATIBILITY AnyNewerVersion)

export(TARGETS ztd_out_ptr FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/ztd/out_ptr-targets.cmake")

file(GLOB_RECURSE ztd_out_ptr_sources
	LIST_DIRECTORIES FALSE
	include/ztd/*.*
)

#target_sources(ztd_out_ptr INTERFACE 
#	$<BUILD_INTERFACE:${ztd_out_ptr_sources}>  
#	$<INSTALL_INTERFACE:${ztd_out_ptr_sources}>
#)
set_target_properties(ztd_out_ptr
	PROPERTIES
	EXPORT_NAME ztd::out_ptr
)

#install(TARGETS ztd_out_ptr
#	EXPORT ztd_out_ptr)

#install(EXPORT ztd_out_ptr
#	FILE ztd_out_ptr-targets.cmake
#	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/ztd/out_ptr")

#install(DIRECTORY "include/ztd/out_ptr"
#	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

#install(FILES
#	"${CMAKE_CURRENT_BINARY_DIR}/cmake/ztd/out_ptr-config.cmake"
#	"${CMAKE_CURRENT_BINARY_DIR}/cmake/ztd/out_ptr-config-version.cmake"
#	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/boost")

if (ZTD_OUT_PTR_BENCHMARKS OR ZTD_OUT_PTR_TESTS OR ZTD_OUT_PTR_EXAMPLES)
	# test and benchmark deps
	FetchContent_Declare(ficapi
		GIT_REPOSITORY https://github.com/ThePhD/ficapi.git
		GIT_TAG origin/main
	)
	FetchContent_MakeAvailable(ficapi)
endif()

if (ZTD_OUT_PTR_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
if (ZTD_OUT_PTR_BENCHMARKS)
	add_subdirectory(benchmarks)
endif()
if (ZTD_OUT_PTR_EXAMPLES)
	add_subdirectory(examples)
endif()

if (ZTD_OUT_PTR_SCRATCH)
	add_executable(ztd.out_ptr.scratch main.cpp)
	target_link_libraries(ztd.out_ptr.scratch
		PRIVATE
		ztd::out_ptr
		d3d11
		dxgi
	)
endif()
