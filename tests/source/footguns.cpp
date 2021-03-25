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

#include <ztd/out_ptr.hpp>

#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>


#if 0
TEST_CASE("out_ptr/footgun", "footguns") {
	SECTION("if") {
		using del = std::default_delete<int>;
		std::shared_ptr<int> meow(nullptr);
		bool value = false;
		if (ficapi_int_create_fail(ztd::out_ptr::out_ptr(meow, del()), 0) == 0 && (value = meow == nullptr) && meow) {
			REQUIRE(false); // this may not happen
		}
		REQUIRE(meow != nullptr);
		REQUIRE_FALSE(value);
	}
	SECTION("if-initializer") {
		using del = std::default_delete<int>;
		std::shared_ptr<int> meow(nullptr);
		bool value = false;
		if (auto err = ficapi_int_create_fail(ztd::out_ptr::out_ptr(meow, del()), 0); err == 0 && (value = meow == nullptr) && meow) {
			REQUIRE(false); // this may not happen
		}
		REQUIRE(meow != nullptr);
		REQUIRE_FALSE(value);
	}
}
#endif // weird footgun cases
