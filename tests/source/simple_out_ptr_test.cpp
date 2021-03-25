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


TEST_CASE("simple_out_ptr/basic", "simple_out_ptr type works with smart pointers and C-style output APIs") {
	SECTION("unique_ptr<void>") {
		std::unique_ptr<void, ficapi::deleter<>> p(nullptr);
		ficapi_create(ztd::out_ptr::op_detail::simple_out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("unique_ptr<int>") {
		std::unique_ptr<int, ficapi::deleter<>> p(nullptr);
		ficapi_int_create(ztd::out_ptr::op_detail::simple_out_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("shared_ptr<void>") {
		std::shared_ptr<void> p(nullptr);
		ficapi_create(ztd::out_ptr::op_detail::simple_out_ptr(p, ficapi::deleter<>()), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("shared_ptr<int>") {
		std::shared_ptr<int> p(nullptr, ficapi_int_delete);
		ficapi_int_create(ztd::out_ptr::op_detail::simple_out_ptr(p, ficapi_int_delete));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
}

TEST_CASE("simple_out_ptr/stateful", "simple_out_ptr type works with stateful smart pointers") {
	SECTION("unique_ptr<void, stateful_deleter>") {
		std::unique_ptr<void, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_create(ztd::out_ptr::op_detail::simple_out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
		REQUIRE(p.get_deleter().type() == ficapi_type::ficapi_type_int);
	}
	SECTION("unique_ptr<int, stateful_int_deleter>") {
		std::unique_ptr<int, ficapi::stateful_int_deleter> p(nullptr, ficapi::stateful_int_deleter{ 0x12345678 });
		ficapi_int_create(ztd::out_ptr::op_detail::simple_out_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("shared_ptr<void>, stateful_deleter") {
		std::shared_ptr<void> p(nullptr, ficapi::stateful_deleter{ 0, ficapi_type::ficapi_type_int });
		ficapi_create(ztd::out_ptr::op_detail::simple_out_ptr(p, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int }), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		ficapi::stateful_deleter* pdel = std::get_deleter<ficapi::stateful_deleter>(p);
		REQUIRE(pdel != nullptr);
		ficapi::stateful_deleter& del = *pdel;
		REQUIRE(del.state() == 0x12345678);
		REQUIRE(del.type() == ficapi_type::ficapi_type_int);
	}
	SECTION("shared_ptr<int>, stateful_int_deleter") {
		std::shared_ptr<int> p(nullptr, ficapi::stateful_int_deleter{ 0 });
		ficapi_int_create(ztd::out_ptr::op_detail::simple_out_ptr(p, ficapi::stateful_int_deleter{ 0x12345678 }));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		ficapi::stateful_int_deleter* pdel = std::get_deleter<ficapi::stateful_int_deleter>(p);
		REQUIRE(pdel != nullptr);
		ficapi::stateful_int_deleter& del = *pdel;
		REQUIRE(del.state() == 0x12345678);
	}
	SECTION("shared_ptr<void>, stateful_deleter ref") {
		ficapi::stateful_deleter del{ 0x12345678, ficapi_type::ficapi_type_int };
		std::shared_ptr<void> p(nullptr, std::ref(del));
		ficapi_create(ztd::out_ptr::op_detail::simple_out_ptr(p, std::ref(del)), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(del.type() == ficapi_type::ficapi_type_int);
		std::reference_wrapper<ficapi::stateful_deleter>* pdel = std::get_deleter<std::reference_wrapper<ficapi::stateful_deleter>>(p);
		REQUIRE(pdel != nullptr);
		REQUIRE(std::addressof(pdel->get()) == std::addressof(del));
		REQUIRE(del.state() == 0x12345678);
	}
	SECTION("shared_ptr<int>, stateful_int_deleter ref") {
		ficapi::stateful_int_deleter del{ 0x12345678 };
		std::shared_ptr<int> p(nullptr, std::ref(del));
		ficapi_int_create(ztd::out_ptr::op_detail::simple_out_ptr(p, std::ref(del)));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		std::reference_wrapper<ficapi::stateful_int_deleter>* pdel = std::get_deleter<std::reference_wrapper<ficapi::stateful_int_deleter>>(p);
		REQUIRE(pdel != nullptr);
		REQUIRE(std::addressof(pdel->get()) == std::addressof(del));
		REQUIRE(del.state() == 0x12345678);
	}
}

TEST_CASE("simple_out_ptr/reused", "simple_out_ptr type properly deletes non-nullptr types from earlier") {
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
		std::unique_ptr<void, reused_deleter> p(nullptr, reused_deleter{});

		ficapi_create(ztd::out_ptr::op_detail::simple_out_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_create(ztd::out_ptr::op_detail::simple_out_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 1);
		}
	}
	SECTION("unique_ptr<int, reused_int_deleter>") {
		std::unique_ptr<int, reused_int_deleter> p(nullptr, reused_int_deleter{});

		ficapi_int_create(ztd::out_ptr::op_detail::simple_out_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_int_create(ztd::out_ptr::op_detail::simple_out_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 1);
		}
	}
}
