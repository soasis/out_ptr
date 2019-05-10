//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#include <boost/out_ptr/inout_ptr.hpp>

#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>


TEST_CASE("simple_inout_ptr/basic", "simple_inout_ptr type works with smart pointers and C-style output APIs") {
	SECTION("unique_ptr<void>") {
		std::unique_ptr<void, ficapi::deleter<>> p(nullptr);
		ficapi_re_create(boost::ptr::out_ptr_detail::simple_inout_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("unique_ptr<int>") {
		std::unique_ptr<int, ficapi::deleter<>> p(nullptr);
		ficapi_int_re_create(boost::ptr::out_ptr_detail::simple_inout_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
}

TEST_CASE("simple_inout_ptr/stateful", "simple_inout_ptr type works with stateful smart pointers") {
	SECTION("unique_ptr<void, stateful_deleter>") {
		std::unique_ptr<void, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_re_create(boost::ptr::out_ptr_detail::simple_inout_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
		REQUIRE(p.get_deleter().type() == ficapi_type::ficapi_type_int);
	}
	SECTION("unique_ptr<int, stateful_int_deleter>") {
		std::unique_ptr<int, ficapi::stateful_int_deleter> p(nullptr, ficapi::stateful_int_deleter{ 0x12345678 });
		ficapi_int_re_create(boost::ptr::out_ptr_detail::simple_inout_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
}

TEST_CASE("simple_inout_ptr/reused", "simple_inout_ptr type properly deletes non-nullptr types from earlier") {
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
	SECTION("unique_ptr<void, reused_deleter>") {
		std::unique_ptr<void, reused_deleter> p(nullptr, reused_deleter{});

		ficapi_re_create(boost::ptr::out_ptr_detail::simple_inout_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_re_create(boost::ptr::out_ptr_detail::simple_inout_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_re_create(boost::ptr::out_ptr_detail::simple_inout_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
	}
	SECTION("unique_ptr<int, reused_int_deleter>") {
		std::unique_ptr<int, reused_int_deleter> p(nullptr, reused_int_deleter{});

		ficapi_int_re_create(boost::ptr::out_ptr_detail::simple_inout_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_int_re_create(boost::ptr::out_ptr_detail::simple_inout_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_int_re_create(boost::ptr::out_ptr_detail::simple_inout_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
	}
}
