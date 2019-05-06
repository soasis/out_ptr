#include <boost/out_ptr/out_ptr.hpp>

#include <boost/handle/handle.hpp>
#include <boost/meta/meta.hpp>
#include <ficapi/ficapi.hpp>

#include <catch2/catch.hpp>

#include <iostream>

namespace boost {
	namespace out_ptr_detail {
		template <typename Handle, typename Pointer, typename Args>
		struct handle_out_ptr
		: voidpp_op<handle_out_ptr<Handle, Pointer, Args>, Pointer>,
		  Args {
		public:
			using Smart = Handle;
			using source_pointer = meta::pointer_of_or_t<Smart, Pointer>;

		private:
			Smart* m_smart_ptr;
			Pointer m_old_ptr;
			Pointer* m_target_ptr;

		public:
			handle_out_ptr(Smart& ptr, Args&& args) noexcept
			: Args(std::move(args)), m_smart_ptr(std::addressof(ptr)), m_old_ptr(static_cast<Pointer>(ptr.get())), m_target_ptr(static_cast<Pointer*>(static_cast<void*>(std::addressof(this->m_smart_ptr->get())))) {
			}

			handle_out_ptr(handle_out_ptr&& right) noexcept
			: Args(std::move(right)), m_smart_ptr(right.m_smart_ptr), m_old_ptr(right.m_old_ptr), m_target_ptr(right.m_target_ptr) {
				right.m_old_ptr == nullptr;
			}
			handle_out_ptr& operator=(handle_out_ptr&& right) noexcept {
				Args::operator=(std::move(right));
				this->m_smart_ptr = right.m_smart_ptr;
				this->m_old_ptr = right.m_old_ptr;
				this->m_target_ptr = right.m_target_ptr;
				right.m_old_ptr == nullptr;
				return *this;
			}
			handle_out_ptr(const handle_out_ptr&) = delete;
			handle_out_ptr& operator=(const handle_out_ptr&) = delete;

			operator Pointer*() noexcept {
				return this->m_target_ptr;
			}
			operator Pointer&() noexcept {
				return *this->m_target_ptr;
			}
			~handle_out_ptr() noexcept {
				reset(std::make_index_sequence<std::tuple_size<Args>::value>());
			}

		private:
			void reset(std::index_sequence<>) {
				if (this->m_old_ptr != nullptr) {
					this->m_smart_ptr->get_deleter()(static_cast<source_pointer>(this->m_old_ptr));
				}
			}

			template <std::size_t I0, std::size_t... I>
			void reset(std::index_sequence<I0, I...>) {
				static_assert(meta::always_false_index_v<I0>, "you cannot reset the deleter for handle<T, Deleter>!: it only takes one argument!");
			}
		};
	} // namespace out_ptr_detail

	template <typename T, typename D, typename Pointer, typename Args>
	struct out_ptr_t<handle<T, D>, Pointer, Args> : out_ptr_detail::handle_out_ptr<handle<T, D>, Pointer, Args> {
	private:
		using core_t = out_ptr_detail::handle_out_ptr<handle<T, D>, Pointer, Args>;

	public:
		using core_t::core_t;
	};

} // namespace boost

TEST_CASE("out_ptr/customization basic", "out_ptr type works with smart pointers and C-style output APIs") {
	SECTION("handle<void*>") {
		boost::handle<void*, ficapi::deleter<>> p(nullptr);
		ficapi_create(boost::out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("handle<int*>") {
		boost::handle<int*, ficapi::int_deleter> p(nullptr);
		ficapi_int_create(boost::out_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
	}
	SECTION("handle<ficapi::opaque*>") {
		boost::handle<ficapi::opaque*, ficapi::handle_deleter> p(nullptr);
		ficapi_handle_create(boost::out_ptr(p));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("handle<ficapi::opaque*>, void out_ptr") {
		boost::handle<ficapi::opaque*, ficapi::handle_deleter> p(nullptr);
		ficapi_create(boost::out_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
	SECTION("handle<void*>, ficapi::opaque_handle out_ptr") {
		boost::handle<void*, ficapi::deleter<ficapi_type::ficapi_type_opaque>> p(nullptr);
		ficapi_create(boost::out_ptr<ficapi::opaque_handle>(p), ficapi::type::ficapi_type_opaque);
		ficapi::opaque_handle rawp = static_cast<ficapi::opaque_handle>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
	}
}

TEST_CASE("out_ptr/customization stateful", "out_ptr type works with stateful deleters in smart pointers") {
	SECTION("handle<void*, stateful_deleter>") {
		boost::handle<void*, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_create(boost::out_ptr(p), ficapi_type::ficapi_type_int);
		int* rawp = static_cast<int*>(p.get());
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<int*, stateful_deleter>") {
		boost::handle<int*, ficapi::stateful_int_deleter> p(nullptr, ficapi::stateful_int_deleter{ 0x12345678 });
		ficapi_int_create(boost::out_ptr(p));
		int* rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(*rawp == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<ficapi::opaque*, stateful_handle_deleter>") {
		boost::handle<ficapi::opaque*, ficapi::stateful_handle_deleter> p(nullptr, ficapi::stateful_handle_deleter{ 0x12345678 });
		ficapi_handle_create(boost::out_ptr(p));
		ficapi::opaque_handle rawp = p.get();
		REQUIRE(rawp != nullptr);
		REQUIRE(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());
		REQUIRE(p.get_deleter().state() == 0x12345678);
	}
	SECTION("handle<ficapi::opaque*, stateful_deleter>, void out_ptr") {
		boost::handle<ficapi::opaque*, ficapi::stateful_deleter> p(nullptr, ficapi::stateful_deleter{ 0x12345678, ficapi_type::ficapi_type_int });
		ficapi_create(boost::out_ptr<void*>(p), ficapi_type::ficapi_type_opaque);
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
		boost::handle<void*, reused_deleter> p(nullptr, reused_deleter{});

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
	SECTION("handle<int*, stateful_deleter>") {
		boost::handle<int*, reused_int_deleter> p(nullptr, reused_int_deleter{});

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
