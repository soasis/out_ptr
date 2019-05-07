//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#include <boost/out_ptr/out_ptr.hpp>

#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>

#include <iostream>

TEST_CASE("clever_out_ptr/basic", "clever_out_ptr type works with smart pointers and C-style output APIs") {
	SECTION("unique_ptr<void>") {
		std::unique_ptr<void, ficapi::deleter<>> p(nullptr);
		ficapi_create(boost::clever_out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("unique_ptr<int>") {
		std::unique_ptr<int, ficapi::deleter<>> p(nullptr);
		ficapi_int_create(boost::clever_out_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
}

TEST_CASE("clever_out_ptr/reused", "clever_out_ptr type properly deletes non-nullptr types from earlier") {
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

		ficapi_create(boost::clever_out_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_create(boost::clever_out_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 1);
		}
	}
	SECTION("unique_ptr<int, stateful_deleter>") {
		std::unique_ptr<int, reused_int_deleter> p(nullptr, reused_int_deleter{});

		ficapi_int_create(boost::clever_out_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_int_create(boost::clever_out_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 1);
		}
	}
}
