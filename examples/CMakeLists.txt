set(phd_out_ptr_example_with_special_unique_sources
	"with_special_unique.cpp"
)

prepend(prefixed_phd_out_ptr_example_with_special_unique_sources "${CMAKE_CURRENT_SOURCE_DIR}/" ${phd_out_ptr_example_with_special_unique_sources})

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
