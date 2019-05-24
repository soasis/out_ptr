//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#include <boost/out_ptr/out_ptr.hpp>

#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>


#include <c_api.hpp>

TEST_CASE("out_ptr/basic", "out_ptr type works with smart pointers and C-style output APIs") {
	SECTION("unique_ptr<void>, ficapi::opaque_handle out_ptr") {
		std::unique_ptr<void, ficapi::deleter<ficapi_type::ficapi_type_opaque>> p(nullptr);
		ficapi_create(boost::out_ptr<ficapi::opaque_handle>(p), ficapi::type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = static_cast<ficapi::opaque_handle>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
}
