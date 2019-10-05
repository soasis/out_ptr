//  C /pyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#include <phd/out_ptr/out_ptr.hpp>

#include <phd/handle.hpp>

#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>

namespace phd { namespace out_ptr {

	// we have null-returning functions in these tests,
	// so we can test this theoretical optimized smart pointer, which is identical to the
	// clever_out_ptr implementation
	template <typename T, typename D, typename Pointer>
	class out_ptr_traits<phd::handle<T, D>, Pointer> {
	private:
		using Smart		 = phd::handle<T, D>;
		using source_pointer = pointer_of_or_t<Smart, Pointer>;

		struct optimized_pointer_state {
			Pointer* target;
			source_pointer old;

			optimized_pointer_state(Pointer* target_ptr, source_pointer old_ptr) noexcept
			: target(target_ptr), old(old_ptr) {
			}

			optimized_pointer_state(optimized_pointer_state&& right) noexcept
			: target(right.target), old(right.old) {
				right.old = nullptr;
			}

			optimized_pointer_state& operator=(optimized_pointer_state&& right) noexcept {
				target    = std::move(right.target);
				right.old = nullptr;
			}
		};

	public:
		using pointer = optimized_pointer_state;

		static pointer construct(Smart& s) noexcept {
			return {
				reinterpret_cast<Pointer*>(std::addressof(s.get())),
				s.get()
			};
		}

		static Pointer* get(Smart&, pointer& p) noexcept {
			return p.target;
		}

		static void reset(Smart& s, pointer& p) {
			if (p.old != nullptr) {
				s.get_deleter()(p.old);
			}
		}
	};

}} // namespace phd::out_ptr

TEST_CASE("out_ptr/customization/traits basic", "out_ptr type works with smart pointers and C-style output APIs") {
	SECTION("handle<void*>") {
		phd::handle<void*, ficapi::deleter<>> p(nullptr);
		ficapi_create(phd::out_ptr::out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("handle<int*>") {
		phd::handle<int*, ficapi::int_deleter> p(nullptr);
		ficapi_int_create(phd::out_ptr::out_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("handle<ficapi::opaque*>") {
		phd::handle<ficapi::opaque*, ficapi::handle_deleter> p(nullptr);
		ficapi_handle_create(phd::out_ptr::out_ptr(p));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("handle<ficapi::opaque*>, void out_ptr") {
		phd::handle<ficapi::opaque*, ficapi::handle_deleter> p(nullptr);
		ficapi_create(phd::out_ptr::out_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("handle<void*>, ficapi::opaque_handle out_ptr") {
		phd::handle<void*, ficapi::deleter<ficapi_type::ficapi_type_opaque>> p(nullptr);
		ficapi_create(phd::out_ptr::out_ptr(p), ficapi::type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = static_cast<ficapi::opaque_handle>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
}

TEST_CASE("out_ptr/customization/traits stateful", "out_ptr type works with stateful deleters in smart pointers") {
	SECTION("handle<void*, stateful_deleter>") {
		phd::handle<void*, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_create(phd::out_ptr::out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<int*, stateful_deleter>") {
		phd::handle<int*, ficapi::stateful_int_deleter> p(nullptr, ficapi::stateful_int_deleter{ 0x12345678 });
		ficapi_int_create(phd::out_ptr::out_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<ficapi::opaque*, stateful_handle_deleter>") {
		phd::handle<ficapi::opaque*, ficapi::stateful_handle_deleter> p(nullptr, ficapi::stateful_handle_deleter{ 0x12345678 });
		ficapi_handle_create(phd::out_ptr::out_ptr(p));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<ficapi::opaque*, stateful_deleter>, void out_ptr") {
		phd::handle<ficapi::opaque*, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_create(phd::out_ptr::out_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
}

TEST_CASE("out_ptr/customization/traits reused", "out_ptr type properly deletes non-nullptr types from earlier") {
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
	SECTION("handle<void*, stateful_deleter>") {
		phd::handle<void*, reused_deleter> p(nullptr, reused_deleter{});

		ficapi_create(phd::out_ptr::out_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_create(phd::out_ptr::out_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 1);
		}
		ficapi_int_create(phd::out_ptr::out_ptr<int*>(p));
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 2);
		}
	}
	SECTION("handle<int*, stateful_deleter>") {
		phd::handle<int*, reused_int_deleter> p(nullptr, reused_int_deleter{});

		ficapi_int_create(phd::out_ptr::out_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_int_create(phd::out_ptr::out_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 1);
		}
		ficapi_create(phd::out_ptr::out_ptr<void*>(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 2);
		}
	}
}
