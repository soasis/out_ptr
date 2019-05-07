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

#include <c_api.hpp>

TEST_CASE("out_ptr/basic", "out_ptr type works with smart pointers and C-style output APIs") {
	SECTION("unique_ptr<void>") {
		std::unique_ptr<void, ficapi::deleter<>> p(nullptr);
		ficapi_create(boost::out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("unique_ptr<int>") {
		std::unique_ptr<int, ficapi::int_deleter> p(nullptr);
		ficapi_int_create(boost::out_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("unique_ptr<ficapi::opaque>") {
		std::unique_ptr<ficapi::opaque, ficapi::handle_deleter> p(nullptr);
		ficapi_handle_create(boost::out_ptr(p));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("unique_ptr<ficapi::opaque>, void out_ptr") {
		std::unique_ptr<ficapi::opaque, ficapi::handle_deleter> p(nullptr);
		ficapi_create(boost::out_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("unique_ptr<void>, ficapi::opaque_handle out_ptr") {
		std::unique_ptr<void, ficapi::deleter<ficapi_type::ficapi_type_opaque>> p(nullptr);
		ficapi_create(boost::out_ptr<ficapi::opaque_handle>(p), ficapi::type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = static_cast<ficapi::opaque_handle>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("shared_ptr<void>") {
		std::shared_ptr<void> p(nullptr);
		ficapi_create(boost::out_ptr(p, ficapi::deleter<>()), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("shared_ptr<int>") {
		std::shared_ptr<int> p(nullptr);
		ficapi_int_create(boost::out_ptr(p, ficapi_int_delete));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("shared_ptr<ficapi::opaque>") {
		std::shared_ptr<ficapi::opaque> p(nullptr);
		ficapi_handle_create(boost::out_ptr(p, ficapi_handle_delete));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("shared_ptr<ficapi::opaque>, void out_ptr") {
		std::shared_ptr<ficapi::opaque> p(nullptr);
		ficapi_create(boost::out_ptr<void*>(p, ficapi_handle_delete), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("shared_ptr<void>, ficapi::opaque_handle out_ptr") {
		std::shared_ptr<void> p(nullptr);
		ficapi_create(boost::out_ptr<ficapi::opaque_handle>(p, ficapi::deleter<ficapi_type::ficapi_type_opaque>()), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = static_cast<ficapi::opaque_handle>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
}

TEST_CASE("out_ptr/stateful", "out_ptr type works with stateful deleters in smart pointers") {
	SECTION("unique_ptr<void, stateful_deleter>") {
		std::unique_ptr<void, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_create(boost::out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("unique_ptr<int, stateful_deleter>") {
		std::unique_ptr<int, ficapi::stateful_int_deleter> p(nullptr, ficapi::stateful_int_deleter{ 0x12345678 });
		ficapi_int_create(boost::out_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("unique_ptr<ficapi::opaque, stateful_handle_deleter>") {
		std::unique_ptr<ficapi::opaque, ficapi::stateful_handle_deleter> p(nullptr, ficapi::stateful_handle_deleter{ 0x12345678 });
		ficapi_handle_create(boost::out_ptr(p));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("unique_ptr<ficapi::opaque, stateful_deleter>, void out_ptr") {
		std::unique_ptr<ficapi::opaque, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_create(boost::out_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("shared_ptr<void>, stateful_deleter") {
		std::shared_ptr<void> p(nullptr, ficapi::stateful_deleter{ 0, ficapi_type::ficapi_type_int });
		ficapi_create(boost::out_ptr(p, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int }), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		ficapi::stateful_deleter* pdel = std::get_deleter<ficapi::stateful_deleter>(p);
		REQUIRE(pdel != nullptr);
		ficapi::stateful_deleter& del = *pdel;
		REQUIRE(del.state() == 0x12345678);
	}
	SECTION("shared_ptr<int>, stateful_int_deleter") {
		std::shared_ptr<int> p(nullptr, ficapi::stateful_int_deleter{ 0 });
		ficapi_int_create(boost::out_ptr(p, ficapi::stateful_int_deleter{ 0x12345678 }));
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
		ficapi_create(boost::out_ptr(p, std::ref(del)), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		std::reference_wrapper<ficapi::stateful_deleter>* pdel = std::get_deleter<std::reference_wrapper<ficapi::stateful_deleter>>(p);
		REQUIRE(pdel != nullptr);
		REQUIRE(std::addressof(pdel->get()) == std::addressof(del));
		REQUIRE(del.state() == 0x12345678);
	}
	SECTION("shared_ptr<int>, stateful_int_deleter ref") {
		ficapi::stateful_int_deleter del{ 0x12345678 };
		std::shared_ptr<int> p(nullptr, std::ref(del));
		ficapi_int_create(boost::out_ptr(p, std::ref(del)));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		std::reference_wrapper<ficapi::stateful_int_deleter>* pdel = std::get_deleter<std::reference_wrapper<ficapi::stateful_int_deleter>>(p);
		REQUIRE(pdel != nullptr);
		REQUIRE(std::addressof(pdel->get()) == std::addressof(del));
		REQUIRE(del.state() == 0x12345678);
	}
}

TEST_CASE("out_ptr/reused", "out_ptr type properly deletes non-nullptr types from earlier") {
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

		ficapi_create(boost::out_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_create(boost::out_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 1);
		}
		ficapi_int_create(boost::out_ptr<int*>(p));
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 2);
		}
	}
	SECTION("unique_ptr<int, stateful_deleter>") {
		std::unique_ptr<int, reused_int_deleter> p(nullptr, reused_int_deleter{});

		ficapi_int_create(boost::out_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_int_create(boost::out_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 1);
		}
		ficapi_create(boost::out_ptr<void*>(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 2);
		}
	}
}

TEST_CASE("out_ptr/simple-bases", "out_ptr will work with statically-castable base/derived relationships") {
	SECTION("base to derived cast") {
		std::unique_ptr<base1> p(nullptr);
		int err = init_derived2(boost::out_ptr<derived2*>(p), false);
		REQUIRE(err == 0);
		REQUIRE(p.get() != nullptr);
	}
}

TEST_CASE("out_ptr/fail", "out_ptr type will static assert various bad usages") {
	// This will fail complication when uncommencted
	// EXPECTED: COMPILE FAIL
	/*
	SECTION("shared without deleter") {
		std::shared_ptr<void> p(nullptr);
		ficapi_create(boost::out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	*/
}
