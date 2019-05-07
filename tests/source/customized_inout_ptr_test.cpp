//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#include <boost/out_ptr/inout_ptr.hpp>
#include <boost/mp11/integer_sequence.hpp>

#include <phd/handle.hpp>

#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>

#include <iostream>

namespace boost {
	template <typename T, typename D, typename Pointer, typename... Args>
	class inout_ptr_t<phd::handle<T, D>, Pointer, Args...> : boost::empty_value<std::tuple<Args...>> {
	private:
		using Smart		 = phd::handle<T, D>;
		using source_pointer = pointer_of_or_t<Smart, Pointer>;
		using ArgsTuple	 = std::tuple<Args...>;
		using Base		 = boost::empty_value<ArgsTuple>;

		Smart* m_smart_ptr;
		Pointer m_old_ptr;
		Pointer* m_target_ptr;

	public:
		inout_ptr_t(Smart& ptr, Args... args) noexcept
		: Base(empty_init_t(), std::forward < decltype<Args>(args)...), m_smart_ptr(std::addressof(ptr)), m_target_ptr(static_cast<Pointer*>(static_cast<void*>(std::addressof(this->m_smart_ptr->get())))) {
		}

		inout_ptr_t(inout_ptr_t&& right) noexcept
		: Args(std::move(right)), m_smart_ptr(right.m_smart_ptr), m_target_ptr(right.m_target_ptr) {
		}
		inout_ptr_t& operator=(inout_ptr_t&& right) noexcept {
			Args::operator	=(std::move(right));
			this->m_smart_ptr  = right.m_smart_ptr;
			this->m_target_ptr = right.m_target_ptr;
			right.m_old_ptr == nullptr;
			return *this;
		}
		inout_ptr_t(const inout_ptr_t&) = delete;
		inout_ptr_t& operator=(const inout_ptr_t&) = delete;

		operator Pointer*() noexcept {
			return this->m_target_ptr;
		}
		operator Pointer&() noexcept {
			return *this->m_target_ptr;
		}

		~inout_ptr_t() noexcept {
			reset(boost::mp11::make_index_sequence<ArgsTuple::value>());
		}

	private:
		void reset(boost::mp11::index_sequence<>) {
			// no need for destructor: aliasing directly, and realloc
		}

		template <std::size_t I0, std::size_t... I>
		void reset(boost::mp11::index_sequence<I0, I...>) {
			static_assert(out_ptr_detail::always_false_index_v<I0>, "you cannot reset the deleter for handle<T, Deleter>!: it only takes one argument!");
		}
	};

} // namespace boost

TEST_CASE("inout_ptr/customization basic", "inout_ptr type works with smart pointers and C-style output APIs") {
	SECTION("handle<void*>") {
		phd::handle<void*, ficapi::deleter<>> p(nullptr);
		ficapi_re_create(boost::inout_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("handle<int*>") {
		phd::handle<int*, ficapi::int_deleter> p(nullptr);
		ficapi_int_re_create(boost::inout_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("handle<ficapi::opaque*>") {
		phd::handle<ficapi::opaque*, ficapi::handle_deleter> p(nullptr);
		ficapi_handle_re_create(boost::inout_ptr(p));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("handle<ficapi::opaque*>, void inout_ptr") {
		phd::handle<ficapi::opaque*, ficapi::handle_deleter> p(nullptr);
		ficapi_re_create(boost::inout_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("handle<void*>, ficapi::opaque_handle inout_ptr") {
		phd::handle<void*, ficapi::deleter<ficapi_type::ficapi_type_opaque>> p(nullptr);
		ficapi_re_create(boost::inout_ptr<ficapi::opaque_handle>(p), ficapi::type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = static_cast<ficapi::opaque_handle>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
}

TEST_CASE("inout_ptr/customization stateful", "inout_ptr type works with stateful deleters in smart pointers") {
	SECTION("handle<void*, stateful_deleter>") {
		phd::handle<void*, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_re_create(boost::inout_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<int*, stateful_deleter>") {
		phd::handle<int*, ficapi::stateful_int_deleter> p(nullptr, ficapi::stateful_int_deleter{ 0x12345678 });
		ficapi_int_re_create(boost::inout_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<ficapi::opaque*, stateful_handle_deleter>") {
		phd::handle<ficapi::opaque*, ficapi::stateful_handle_deleter> p(nullptr, ficapi::stateful_handle_deleter{ 0x12345678 });
		ficapi_handle_re_create(boost::inout_ptr(p));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<ficapi::opaque*, stateful_deleter>, void inout_ptr") {
		phd::handle<ficapi::opaque*, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_re_create(boost::inout_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
}

TEST_CASE("inout_ptr/customization reused", "inout_ptr type properly deletes non-nullptr types from earlier") {
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

		ficapi_re_create(boost::inout_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_re_create(boost::inout_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_int_re_create(boost::inout_ptr<int*>(p));
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
	}
	SECTION("handle<int*, stateful_deleter>") {
		phd::handle<int*, reused_int_deleter> p(nullptr, reused_int_deleter{});

		ficapi_int_re_create(boost::inout_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_int_re_create(boost::inout_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_re_create(boost::inout_ptr<void*>(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
	}
}
