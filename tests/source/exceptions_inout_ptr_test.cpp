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

#include <ztd/out_ptr/inout_ptr.hpp>

#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>


TEST_CASE("inout_ptr/exceptions/basic fail in scope", "inout_ptr does not double-delete or destroy a pointer if the C function fails") {
	SECTION("unique_ptr<void>") {
		std::unique_ptr<void, ficapi::deleter<>> p(nullptr);
		ficapi_re_create(ztd::out_ptr::inout_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		try {
			if (ficapi_re_create_fail(ztd::out_ptr::inout_ptr(p), ficapi_type::ficapi_type_int, 1) != 0) {
				throw std::runtime_error("ficapi: failure to create");
			}
		}
		catch (const std::runtime_error&) {
			REQUIRE(rawp == p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
		}
	}
	SECTION("unique_ptr<int>") {
		std::unique_ptr<int, ficapi::int_deleter> p(nullptr);
		ficapi_int_re_create(ztd::out_ptr::inout_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		try {
			if (ficapi_int_re_create_fail(ztd::out_ptr::inout_ptr(p), 1) != 0) {
				throw std::runtime_error("ficapi: failure to create");
			}
		}
		catch (const std::runtime_error&) {
			REQUIRE(rawp == p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
		}
	}
	SECTION("unique_ptr<ficapi::opaque>") {
		std::unique_ptr<ficapi::opaque, ficapi::handle_deleter> p(nullptr);
		ficapi_handle_re_create(ztd::out_ptr::inout_ptr(p));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		try {
			if (ficapi_handle_re_create_fail(ztd::out_ptr::inout_ptr(p), 1) != 0) {
				throw std::runtime_error("ficapi: failure to create");
			}
		}
		catch (const std::runtime_error&) {
			REQUIRE(rawp == p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		}
	}
	SECTION("unique_ptr<ficapi::opaque>, void inout_ptr") {
		std::unique_ptr<ficapi::opaque, ficapi::handle_deleter> p(nullptr);
		ficapi_re_create(ztd::out_ptr::inout_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		try {
			if (ficapi_re_create_fail(ztd::out_ptr::inout_ptr<void*>(p), ficapi_type::ficapi_type_opaque, 1) != 0) {
				throw std::runtime_error("ficapi: failure to create");
			}
		}
		catch (const std::runtime_error&) {
			REQUIRE(rawp == p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		}
	}
}

TEST_CASE("inout_ptr/exceptions/stateful fail in scope", "inout_ptr type works with stateful deleters in smart pointers") {
	SECTION("unique_ptr<void, stateful_deleter>") {
		std::unique_ptr<void, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_re_create(ztd::out_ptr::inout_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
		try {
			if (ficapi_re_create_fail(ztd::out_ptr::inout_ptr(p), ficapi_type::ficapi_type_int, 1) != 0) {
				throw std::runtime_error("ficapi: failure to create");
			}
		}
		catch (const std::runtime_error&) {
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().state() == 0x12345678);
		}
	}
	SECTION("unique_ptr<int, stateful_deleter>") {
		std::unique_ptr<int, ficapi::stateful_int_deleter> p(nullptr, ficapi::stateful_int_deleter{ 0x12345678 });
		ficapi_int_re_create(ztd::out_ptr::inout_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
		try {
			if (ficapi_int_re_create_fail(ztd::out_ptr::inout_ptr(p), 1) != 0) {
				throw std::runtime_error("ficapi: failure to create");
			}
		}
		catch (const std::runtime_error&) {
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().state() == 0x12345678);
		}
	}
	SECTION("unique_ptr<ficapi::opaque, stateful_handle_deleter>") {
		std::unique_ptr<ficapi::opaque, ficapi::stateful_handle_deleter> p(nullptr, ficapi::stateful_handle_deleter{ 0x12345678 });
		ficapi_handle_re_create(ztd::out_ptr::inout_ptr(p));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
		try {
			if (ficapi_handle_re_create_fail(ztd::out_ptr::inout_ptr(p), 1) != 0) {
				throw std::runtime_error("ficapi: failure to create");
			}
		}
		catch (const std::runtime_error&) {
			REQUIRE(rawp != nullptr);
			REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().state() == 0x12345678);
		}
	}
	SECTION("unique_ptr<ficapi::opaque, stateful_deleter>, void inout_ptr") {
		std::unique_ptr<ficapi::opaque, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_re_create(ztd::out_ptr::inout_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
		try {
			if (ficapi_re_create_fail(ztd::out_ptr::inout_ptr<void*>(p), ficapi_type::ficapi_type_opaque, 1) != 0) {
				throw std::runtime_error("ficapi: failure to create");
			}
		}
		catch (const std::runtime_error&) {
			REQUIRE(rawp != nullptr);
			REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().state() == 0x12345678);
		}
	}
}

TEST_CASE("inout_ptr/exceptions/exception scope escape", "inout_ptr type properly deletes non-nullptr types from earlier") {
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
		reused_deleter deleter{};
		try {
			std::unique_ptr<void, std::reference_wrapper<reused_deleter>> p(nullptr, std::ref(deleter));

			ficapi_re_create(ztd::out_ptr::inout_ptr(p), ficapi_type::ficapi_type_int);
			{
				int* rawp = static_cast<int*>(p.get());
				REQUIRE(rawp != nullptr);
				REQUIRE(*rawp == ficapi_get_dynamic_data());
				REQUIRE(std::addressof(p.get_deleter().get()) == std::addressof(deleter));
				REQUIRE(p.get_deleter().get().store == 0);
			}
			ficapi_re_create(ztd::out_ptr::inout_ptr(p), ficapi_type::ficapi_type_int);
			{
				int* rawp = static_cast<int*>(p.get());
				REQUIRE(rawp != nullptr);
				REQUIRE(*rawp == ficapi_get_dynamic_data());
				REQUIRE(std::addressof(p.get_deleter().get()) == std::addressof(deleter));
				REQUIRE(p.get_deleter().get().store == 0);
			}
			if (ficapi_re_create_fail(ztd::out_ptr::inout_ptr<void*>(p), ficapi_type::ficapi_type_opaque, 1) != 0) {
				int* rawp = static_cast<int*>(p.get());
				REQUIRE(rawp != nullptr);
				REQUIRE(*rawp == ficapi_get_dynamic_data());
				REQUIRE(std::addressof(p.get_deleter().get()) == std::addressof(deleter));
				REQUIRE(p.get_deleter().get().store == 0);
				throw std::runtime_error("ficapi: failure to create");
			}
		}
		catch (const std::runtime_error&) {
			REQUIRE(deleter.store == 1);
		}
	}
	SECTION("unique_ptr<int, stateful_deleter>") {
		reused_int_deleter deleter{};
		try {
			std::unique_ptr<int, std::reference_wrapper<reused_int_deleter>> p(nullptr, std::ref(deleter));

			ficapi_int_re_create(ztd::out_ptr::inout_ptr(p));
			{
				int* rawp = p.get();
				REQUIRE(rawp != nullptr);
				REQUIRE(*rawp == ficapi_get_dynamic_data());
				REQUIRE(std::addressof(p.get_deleter().get()) == std::addressof(deleter));
				REQUIRE(p.get_deleter().get().store == 0);
			}
			ficapi_int_re_create(ztd::out_ptr::inout_ptr(p));
			{
				int* rawp = p.get();
				REQUIRE(rawp != nullptr);
				REQUIRE(*rawp == ficapi_get_dynamic_data());
				REQUIRE(std::addressof(p.get_deleter().get()) == std::addressof(deleter));
				REQUIRE(p.get_deleter().get().store == 0);
			}
			if (ficapi_int_re_create_fail(ztd::out_ptr::inout_ptr(p), 1) != 0) {
				int* rawp = p.get();
				REQUIRE(rawp != nullptr);
				REQUIRE(*rawp == ficapi_get_dynamic_data());
				REQUIRE(std::addressof(p.get_deleter().get()) == std::addressof(deleter));
				REQUIRE(p.get_deleter().get().store == 0);
				throw std::runtime_error("ficapi: failure to create");
			}
		}
		catch (const std::runtime_error&) {
			REQUIRE(deleter.store == 1);
		}
	}
}
