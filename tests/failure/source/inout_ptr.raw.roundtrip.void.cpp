


//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#include <boost/out_ptr/inout_ptr.hpp>

#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>


TEST_CASE("inout_ptr/failure/raw/roundtrip/void", "inout_ptr type should not roundtrip void** -> Pointer* -> void**, with raw void") {
	SECTION("void*, ficapi::opaque_handle inout_ptr") {
		void* p(nullptr);
		ficapi_re_create(boost::out_ptr::inout_ptr<ficapi::opaque_handle>(p), ficapi::type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = static_cast<ficapi::opaque_handle>(p);
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		ficapi_delete(p, ficapi_type::ficapi_type_opaque);
	}
}
