#include <boost/out_ptr/inout_ptr.hpp>

#include <boost/handle/handle.hpp>
#include <boost/meta/meta.hpp>
#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>

#include <iostream>

namespace boost {
	namespace out_ptr_detail {
		template <typename Handle, typename Pointer, typename Args>
		struct handle_inout_ptr
		: voidpp_op<handle_inout_ptr<Handle, Pointer, Args>, Pointer>,
		  Args {
		public:
			using Smart = Handle;
			using source_pointer = meta::pointer_of_or_t<Smart, Pointer>;

		private:
			Smart* m_smart_ptr;
			Pointer* m_target_ptr;

		public:
			handle_inout_ptr(Smart& ptr, Args&& args) noexcept
			: Args(std::move(args)), m_smart_ptr(std::addressof(ptr)), m_target_ptr(static_cast<Pointer*>(static_cast<void*>(std::addressof(this->m_smart_ptr->get())))) {
			}

			handle_inout_ptr(handle_inout_ptr&& right) noexcept
			: Args(std::move(right)), m_smart_ptr(right.m_smart_ptr), m_target_ptr(right.m_target_ptr) {
			}
			handle_inout_ptr& operator=(handle_inout_ptr&& right) noexcept {
				Args::operator=(std::move(right));
				this->m_smart_ptr = right.m_smart_ptr;
				this->m_target_ptr = right.m_target_ptr;
				right.m_old_ptr == nullptr;
				return *this;
			}
			handle_inout_ptr(const handle_inout_ptr&) = delete;
			handle_inout_ptr& operator=(const handle_inout_ptr&) = delete;

			operator Pointer*() noexcept {
				return this->m_target_ptr;
			}
			operator Pointer&() noexcept {
				return *this->m_target_ptr;
			}

			~handle_inout_ptr() noexcept {
				reset(std::make_index_sequence<std::tuple_size<Args>::value>());
			}

		private:
			void reset(std::index_sequence<>) {
				// no need for destructor: aliasing directly, and realloc
			}

			template <std::size_t I0, std::size_t... I>
			void reset(std::index_sequence<I0, I...>) {
				static_assert(meta::always_false_index_v<I0>, "you cannot reset the deleter for handle<T, Deleter>!: it only takes one argument!");
			}
		};
	} // namespace out_ptr_detail

	template <typename T, typename D, typename Pointer, typename... Args>
	struct inout_ptr_t<handle<T, D>, Pointer, Args...> : out_ptr_detail::handle_inout_ptr<handle<T, D>, Pointer, std::tuple<Args...>> {
	private:
		using Smart = handle<T, D>;
		using core_t = out_ptr_detail::handle_inout_ptr<Smart, Pointer, std::tuple<Args...>>;

	public:
		inout_ptr_t(Smart& s, Args... args)
		: core_t(s, std::forward_as_tuple(std::forward<Args>(args)...)) {
		}
	};

} // namespace boost

TEST_CASE("inout_ptr/customization basic", "inout_ptr type works with smart pointers and C-style output APIs") {
	SECTION("handle<void*>") {
		boost::handle<void*, ficapi::deleter<>> p(nullptr);
		ficapi_re_create(boost::inout_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("handle<int*>") {
		boost::handle<int*, ficapi::int_deleter> p(nullptr);
		ficapi_int_re_create(boost::inout_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("handle<ficapi::opaque*>") {
		boost::handle<ficapi::opaque*, ficapi::handle_deleter> p(nullptr);
		ficapi_handle_re_create(boost::inout_ptr(p));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("handle<ficapi::opaque*>, void inout_ptr") {
		boost::handle<ficapi::opaque*, ficapi::handle_deleter> p(nullptr);
		ficapi_re_create(boost::inout_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("handle<void*>, ficapi::opaque_handle inout_ptr") {
		boost::handle<void*, ficapi::deleter<ficapi_type::ficapi_type_opaque>> p(nullptr);
		ficapi_re_create(boost::inout_ptr<ficapi::opaque_handle>(p), ficapi::type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = static_cast<ficapi::opaque_handle>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
}

TEST_CASE("inout_ptr/customization stateful", "inout_ptr type works with stateful deleters in smart pointers") {
	SECTION("handle<void*, stateful_deleter>") {
		boost::handle<void*, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_re_create(boost::inout_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<int*, stateful_deleter>") {
		boost::handle<int*, ficapi::stateful_int_deleter> p(nullptr, ficapi::stateful_int_deleter{ 0x12345678 });
		ficapi_int_re_create(boost::inout_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<ficapi::opaque*, stateful_handle_deleter>") {
		boost::handle<ficapi::opaque*, ficapi::stateful_handle_deleter> p(nullptr, ficapi::stateful_handle_deleter{ 0x12345678 });
		ficapi_handle_re_create(boost::inout_ptr(p));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<ficapi::opaque*, stateful_deleter>, void inout_ptr") {
		boost::handle<ficapi::opaque*, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
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
		boost::handle<void*, reused_deleter> p(nullptr, reused_deleter{});

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
		boost::handle<int*, reused_int_deleter> p(nullptr, reused_int_deleter{});

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
