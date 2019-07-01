//  C /pyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#include <boost/out_ptr/out_ptr.hpp>
#include <boost/mp11/integer_sequence.hpp>

#include <phd/handle.hpp>

#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>

// A template that always evaluates to false anyhow
template <std::size_t I>
struct always_false_index : std::integral_constant<bool, I == 1 && I == 0> {};

namespace phd {
	template <typename T, typename D>
	class special_out_customization_handle : public handle<T, D> {
	private:
		using base_t = handle<T, D>;

	public:
		using base_t::base_t;
	};
} // namespace phd

namespace boost { namespace out_ptr {

	// we have null-returning functions in these tests,
	// so we can test this theoretical optimized smart pointer, which is identical to the
	// clever_out_ptr implementation
	template <typename T, typename D, typename Pointer, typename... Args>
	class out_ptr_t<phd::special_out_customization_handle<T, D>, Pointer, Args...> : boost::empty_value<std::tuple<Args...>> {
	private:
		using Smart		 = phd::special_out_customization_handle<T, D>;
		using source_pointer = pointer_of_or_t<Smart, Pointer>;
		using ArgsTuple	 = std::tuple<Args...>;
		using Base		 = boost::empty_value<ArgsTuple>;

		Smart* m_smart_ptr;
		source_pointer m_old_ptr;
		Pointer* m_target_ptr;

	public:
		out_ptr_t(Smart& s, Args... args) noexcept
		: Base(empty_init_t(), std::forward<Args>(args)...), m_smart_ptr(std::addressof(s)), m_old_ptr(s.get()), m_target_ptr(reinterpret_cast<Pointer*>(std::addressof(this->m_smart_ptr->get()))) {
		}

		out_ptr_t(out_ptr_t&& right) noexcept
		: Base(std::move(right)), m_smart_ptr(right.m_smart_ptr), m_old_ptr(right.m_old_ptr), m_target_ptr(right.m_target_ptr) {
			right.m_old_ptr = nullptr;
		}
		out_ptr_t& operator=(out_ptr_t&& right) noexcept {
			Base::operator	=(std::move(right));
			this->m_smart_ptr  = right.m_smart_ptr;
			this->m_old_ptr    = right.m_old_ptr;
			this->m_target_ptr = right.m_target_ptr;
			right.m_old_ptr    = nullptr;
			return *this;
		}

		operator Pointer*() const noexcept {
			return const_cast<Pointer*>(this->m_target_ptr);
		}

		~out_ptr_t() noexcept {
			reset(boost::mp11::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>());
		}

	private:
		void reset(boost::mp11::index_sequence<>) {
			if (this->m_old_ptr != nullptr) {
				this->m_smart_ptr->get_deleter()(this->m_old_ptr);
			}
		}

		template <std::size_t I0, std::size_t... I>
		void reset(boost::mp11::index_sequence<I0, I...>) {
			static_assert(always_false_index<I0>::value, "you cannot reset the deleter for handle<T, Deleter>!: it only takes one argument!");
		}
	};

}} // namespace boost::out_ptr

TEST_CASE("out_ptr/customization basic", "out_ptr type works with smart pointers and C-style output APIs") {
	SECTION("handle<void*>") {
		phd::special_out_customization_handle<void*, ficapi::deleter<>> p(nullptr);
		ficapi_create(boost::out_ptr::out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("handle<int*>") {
		phd::special_out_customization_handle<int*, ficapi::int_deleter> p(nullptr);
		ficapi_int_create(boost::out_ptr::out_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("handle<ficapi::opaque*>") {
		phd::special_out_customization_handle<ficapi::opaque*, ficapi::handle_deleter> p(nullptr);
		ficapi_handle_create(boost::out_ptr::out_ptr(p));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("handle<ficapi::opaque*>, void out_ptr") {
		phd::special_out_customization_handle<ficapi::opaque*, ficapi::handle_deleter> p(nullptr);
		ficapi_create(boost::out_ptr::out_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("handle<void*>, ficapi::opaque_handle out_ptr") {
		phd::special_out_customization_handle<void*, ficapi::deleter<ficapi_type::ficapi_type_opaque>> p(nullptr);
		ficapi_create(boost::out_ptr::out_ptr(p), ficapi::type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = static_cast<ficapi::opaque_handle>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
}

TEST_CASE("out_ptr/customization stateful", "out_ptr type works with stateful deleters in smart pointers") {
	SECTION("handle<void*, stateful_deleter>") {
		phd::special_out_customization_handle<void*, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_create(boost::out_ptr::out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<int*, stateful_deleter>") {
		phd::special_out_customization_handle<int*, ficapi::stateful_int_deleter> p(nullptr, ficapi::stateful_int_deleter{ 0x12345678 });
		ficapi_int_create(boost::out_ptr::out_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<ficapi::opaque*, stateful_handle_deleter>") {
		phd::special_out_customization_handle<ficapi::opaque*, ficapi::stateful_handle_deleter> p(nullptr, ficapi::stateful_handle_deleter{ 0x12345678 });
		ficapi_handle_create(boost::out_ptr::out_ptr(p));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<ficapi::opaque*, stateful_deleter>, void out_ptr") {
		phd::special_out_customization_handle<ficapi::opaque*, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_create(boost::out_ptr::out_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
}

TEST_CASE("out_ptr/customization reused", "out_ptr type properly deletes non-nullptr types from earlier") {
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
		phd::special_out_customization_handle<void*, reused_deleter> p(nullptr, reused_deleter{});

		ficapi_create(boost::out_ptr::out_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_create(boost::out_ptr::out_ptr(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 1);
		}
		ficapi_int_create(boost::out_ptr::out_ptr<int*>(p));
		{
			int* rawp = static_cast<int*>(p.get());
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 2);
		}
	}
	SECTION("handle<int*, stateful_deleter>") {
		phd::special_out_customization_handle<int*, reused_int_deleter> p(nullptr, reused_int_deleter{});

		ficapi_int_create(boost::out_ptr::out_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 0);
		}
		ficapi_int_create(boost::out_ptr::out_ptr(p));
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 1);
		}
		ficapi_create(boost::out_ptr::out_ptr<void*>(p), ficapi_type::ficapi_type_int);
		{
			int* rawp = p.get();
			REQUIRE(rawp != nullptr);
			REQUIRE(*rawp == ficapi_get_dynamic_data());
			REQUIRE(p.get_deleter().store == 2);
		}
	}
}
