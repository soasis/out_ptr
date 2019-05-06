set(boost_out_ptr_tests_sources
	"source/clever_out_ptr_test.cpp"
	"source/simple_out_ptr_test.cpp"
	"source/out_ptr_test.cpp"
	"source/raw_out_ptr_test.cpp"
	"source/customized_out_ptr_test.cpp"
	"source/custom_unique_deleter_test.cpp"

	"source/clever_inout_ptr_test.cpp"
	"source/simple_inout_ptr_test.cpp"
	"source/inout_ptr_test.cpp"
	"source/customized_inout_ptr_test.cpp"

	"source/out_ptr_tests_main.cpp"
)

add_executable(boost_out_ptr_tests ${boost_out_ptr_tests_sources})
if (MSVC)
	# just use the compiler's default,
	# since we can't pick C++11 directly
	#target_compile_options(boost_out_ptr_tests
	#	PRIVATE /std:c++latest)
else()
	target_compile_options(boost_out_ptr_tests
		PRIVATE -std=c++11)
endif()
target_include_directories(boost_out_ptr_tests
	PRIVATE 
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_SOURCE_DIR}/vendor/catch2/single_include")
target_link_libraries(boost_out_ptr_tests 
	PRIVATE boost::boost ficapi ${CMAKE_DL_LIBS}
)
add_test(NAME boost_out_ptr_tests COMMAND boost_out_ptr_tests)
