# Copyright Ⓒ 2018-2019 ThePhD
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

# Partial (add_subdirectory only) and experimental CMake support
# Subject to change; please do not rely on the contents of this file yet

set(phd_out_ptr_example_with_special_unique_sources
	"boost.shared_ptr.cpp"
	"boost.intrusive_ptr.cpp"
	"std.shared_ptr.cpp"
	"std.unique_ptr.cpp"
	"std.custom_unique_ptr.cpp"
)

add_executable(phd_out_ptr_example_with_special_unique ${prefixed_phd_out_ptr_example_with_special_unique_sources})
if (MSVC)
	target_compile_options(phd_out_ptr_example_with_special_unique
		PRIVATE /std:c++latest)
else()
	target_compile_options(phd_out_ptr_example_with_special_unique
		PRIVATE -std=c++1z)
endif()
target_compile_definitions(phd_out_ptr_example_with_special_unique PRIVATE __STDC_WANT_LIB_EXT1__=1)
target_link_libraries(phd_out_ptr_example_with_special_unique 
	PRIVATE phd::phd ${CMAKE_DL_LIBS}
)
if (OUT_PTR_TESTS)
	add_test(NAME phd_out_ptr_example_with_special_unique COMMAND phd_out_ptr_example_with_special_unique)
endif()
