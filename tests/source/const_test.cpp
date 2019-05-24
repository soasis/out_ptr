//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#include <boost/out_ptr/out_ptr.hpp>
#include <boost/out_ptr/inout_ptr.hpp>

#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>

// simulate C++03-era wrappers
template <typename... Args>
void cxx_03_wrapper_function(const Args&... args) {
	ficapi_create(args...);
}

// simulate C++03-era wrappers
template <typename... Args>
void cxx_03_re_wrapper_function(const Args&... args) {
	ficapi_re_create(args...);
}

TEST_CASE("out_ptr/const wrappers", "out_ptr works with const APIs") {
	SECTION("unique_ptr<void>") {
		std::unique_ptr<void, ficapi::deleter<>> p(nullptr);
		cxx_03_wrapper_function(boost::out_ptr::out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
}

TEST_CASE("inout_ptr/const wrappers", "out_ptr works with const APIs") {
	SECTION("unique_ptr<void>") {
		std::unique_ptr<void, ficapi::deleter<>> p(nullptr);
		cxx_03_re_wrapper_function(boost::out_ptr::inout_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
}
