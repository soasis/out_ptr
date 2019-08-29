//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#include <phd/out_ptr.hpp>

#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>


#if 0
TEST_CASE("out_ptr/footgun", "footguns") {
	SECTION("if") {
		using del = std::default_delete<int>;
		std::shared_ptr<int> meow(nullptr);
		bool value = false;
		if (ficapi_int_create_fail(phd::out_ptr::out_ptr(meow, del()), 0) == 0 && (value = meow == nullptr) && meow) {
			REQUIRE(false); // this may not happen
		}
		REQUIRE(meow != nullptr);
		REQUIRE_FALSE(value);
	}
	SECTION("if-initializer") {
		using del = std::default_delete<int>;
		std::shared_ptr<int> meow(nullptr);
		bool value = false;
		if (auto err = ficapi_int_create_fail(phd::out_ptr::out_ptr(meow, del()), 0); err == 0 && (value = meow == nullptr) && meow) {
			REQUIRE(false); // this may not happen
		}
		REQUIRE(meow != nullptr);
		REQUIRE_FALSE(value);
	}
}
#endif // weird footgun cases
