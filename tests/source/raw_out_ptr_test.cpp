//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#include <phd/out_ptr/out_ptr.hpp>

#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>


TEST_CASE("out_ptr/raw", "out_ptr type works with raw pointers and c-style output APIs") {
	SECTION("void*") {
		void* p = nullptr;
		ficapi_create(phd::out_ptr::out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p);
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		ficapi_delete(p, ficapi_type::ficapi_type_int);
	}
	SECTION("int*") {
		int* p(nullptr);
		ficapi_int_create(phd::out_ptr::out_ptr(p));
		int* rawp = p;
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		ficapi_int_delete(p);
	}
	SECTION("ficapi::opaque*") {
		ficapi::opaque* p(nullptr);
		ficapi_handle_create(phd::out_ptr::out_ptr(p));
		ficapi::opaque_handle rawp = p;
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		ficapi_handle_delete(p);
	}
	SECTION("ficapi::opaque*, void out_ptr") {
		ficapi::opaque* p(nullptr);
		ficapi_create(phd::out_ptr::out_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p;
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		ficapi_handle_delete(p);
	}
}

TEST_CASE("out_ptr/raw/void roundtrip", "esnure raw pointers can be roundtripped through the void conversion") {
	SECTION("void*, ficapi::opaque_handle out_ptr") {
		void* p(nullptr);
		ficapi_create(phd::out_ptr::out_ptr<ficapi::opaque_handle>(p), ficapi::type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = static_cast<ficapi::opaque_handle>(p);
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
}
