// Copyright ⓒ 2018-2021 ThePhD.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#include <ztd/out_ptr/out_ptr.hpp>

#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>


TEST_CASE("out_ptr/raw", "out_ptr type works with raw pointers and c-style output APIs") {
	SECTION("void*") {
		void* p = nullptr;
		ficapi_create(ztd::out_ptr::out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p);
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		ficapi_delete(p, ficapi_type::ficapi_type_int);
	}
	SECTION("int*") {
		int* p(nullptr);
		ficapi_int_create(ztd::out_ptr::out_ptr(p));
		int* rawp = p;
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		ficapi_int_delete(p);
	}
	SECTION("ficapi::opaque*") {
		ficapi::opaque* p(nullptr);
		ficapi_handle_create(ztd::out_ptr::out_ptr(p));
		ficapi::opaque_handle rawp = p;
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		ficapi_handle_delete(p);
	}
	SECTION("ficapi::opaque*, void out_ptr") {
		ficapi::opaque* p(nullptr);
		ficapi_create(ztd::out_ptr::out_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p;
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		ficapi_handle_delete(p);
	}
}

TEST_CASE("out_ptr/raw/void roundtrip", "esnure raw pointers can be roundtripped through the void conversion") {
	SECTION("void*, ficapi::opaque_handle out_ptr") {
		void* p(nullptr);
		ficapi_create(ztd::out_ptr::out_ptr<ficapi::opaque_handle>(p), ficapi::type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = static_cast<ficapi::opaque_handle>(p);
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
}
