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


TEST_CASE("out_ptr/exceptions/reused", "out_ptr type properly deletes non-nullptr types from earlier") {
	struct reused_deleter {
		int store = 0;

		void operator()(void* x) {
			++store;
			ficapi_delete(x, ficapi_type::ficapi_type_int);
		}
	};
	struct reused_int_deleter {
		int store = 0;

		void operator()(int* x) {
			++store;
			ficapi_int_delete(x);
		}
	};
	SECTION("unique_ptr<void, stateful_deleter>") {
		reused_deleter deleter {};
		try {
			std::unique_ptr<void, std::reference_wrapper<reused_deleter>> p(nullptr, std::ref(deleter));

			ficapi_create(ztd::out_ptr::out_ptr(p), ficapi_type::ficapi_type_int);
			{
				int* rawp = static_cast<int*>(p.get());
				REQUIRE(rawp != nullptr);
				REQUIRE(*rawp == ficapi_get_dynamic_data());
				REQUIRE(std::addressof(p.get_deleter().get()) == std::addressof(deleter));
				REQUIRE(p.get_deleter().get().store == 0);
			}
			ficapi_create(ztd::out_ptr::out_ptr(p), ficapi_type::ficapi_type_int);
			{
				int* rawp = static_cast<int*>(p.get());
				REQUIRE(rawp != nullptr);
				REQUIRE(*rawp == ficapi_get_dynamic_data());
				REQUIRE(std::addressof(p.get_deleter().get()) == std::addressof(deleter));
				REQUIRE(p.get_deleter().get().store == 1);
			}
			{
				int* before_rawp = static_cast<int*>(p.get());
				int err		  = ficapi_int_create_fail(ztd::out_ptr::out_ptr<int*>(p), 1);
				if (err == 1) {
					REQUIRE(err == 1);
					int* rawp = static_cast<int*>(p.get());
					REQUIRE(rawp == before_rawp);
					REQUIRE(std::addressof(p.get_deleter().get()) == std::addressof(deleter));
					REQUIRE(p.get_deleter().get().store == 1);
					throw std::runtime_error("ficapi: failure to create");
				}
				else {
					REQUIRE(err == 0);
					int* rawp = static_cast<int*>(p.get());
					REQUIRE(rawp != before_rawp);
					REQUIRE(rawp != nullptr);
					REQUIRE(std::addressof(p.get_deleter().get()) == std::addressof(deleter));
					REQUIRE(p.get_deleter().get().store == 2);
					throw std::runtime_error("ficapi: failure to create");
				}
			}
		}
		catch (const std::runtime_error&) {
			REQUIRE(deleter.store == 2);
		}
	}
	SECTION("unique_ptr<int, stateful_deleter>") {
		reused_int_deleter deleter;
		try {
			std::unique_ptr<int, std::reference_wrapper<reused_int_deleter>> p(nullptr, std::ref(deleter));

			ficapi_int_create(ztd::out_ptr::out_ptr(p));
			{
				int* rawp = p.get();
				REQUIRE(rawp != nullptr);
				REQUIRE(*rawp == ficapi_get_dynamic_data());
				REQUIRE(std::addressof(p.get_deleter().get()) == std::addressof(deleter));
				REQUIRE(p.get_deleter().get().store == 0);
			}
			ficapi_int_create(ztd::out_ptr::out_ptr(p));
			{
				int* rawp = p.get();
				REQUIRE(rawp != nullptr);
				REQUIRE(*rawp == ficapi_get_dynamic_data());
				REQUIRE(std::addressof(p.get_deleter().get()) == std::addressof(deleter));
				REQUIRE(p.get_deleter().get().store == 1);
			}
			{
				int* before_rawp = static_cast<int*>(p.get());
				int err		  = ficapi_create_fail(ztd::out_ptr::out_ptr<void*>(p), ficapi_type::ficapi_type_int, 1);
				if (err == 1) {
					REQUIRE(err == 1);
					int* rawp = static_cast<int*>(p.get());
					REQUIRE(rawp == before_rawp);
					REQUIRE(std::addressof(p.get_deleter().get()) == std::addressof(deleter));
					REQUIRE(p.get_deleter().get().store == 1);
					throw std::runtime_error("ficapi: failure to create");
				}
				else {
					REQUIRE(err == 0);
					int* rawp = static_cast<int*>(p.get());
					REQUIRE(rawp != before_rawp);
					REQUIRE(rawp != nullptr);
					REQUIRE(std::addressof(p.get_deleter().get()) == std::addressof(deleter));
					REQUIRE(p.get_deleter().get().store == 2);
					throw std::runtime_error("ficapi: failure to create");
				}
			}
		}
		catch (const std::runtime_error&) {
			REQUIRE(deleter.store == 2);
		}
	}
}
