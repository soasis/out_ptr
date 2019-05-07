//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.


#include <phd/handle.hpp>

#include <ficapi/ficapi.hpp>

#include <boost/out_ptr.hpp>
#include <boost/mp11/integer_sequence.hpp>

#include <type_traits>

// A template that always evaluates to false anyhow
template <std::size_t>
using always_false_index = std::false_type;

namespace boost {

	template <typename T, typename D, typename Pointer, typename... Args>
	class out_ptr_t<phd::handle<T, D>, Pointer, Args...> : boost::empty_value<std::tuple<Args...>> {
	private:
		using Smart		 = phd::handle<T, D>;
		using source_pointer = pointer_of_or_t<Smart, Pointer>;
		using Base		 = boost::empty_value<std::tuple<Args...>>;

		Smart* m_smart_ptr;
		Pointer m_old_ptr;
		Pointer* m_target_ptr;

	public:
		out_ptr_t(Smart& ptr, Args... args) noexcept
		: Base(std::forward<decltype(Args)>(args)...), m_smart_ptr(std::addressof(ptr)), m_old_ptr(static_cast<Pointer>(ptr.get())), m_target_ptr(static_cast<Pointer*>(static_cast<void*>(std::addressof(this->m_smart_ptr->get())))) {
		}

		out_ptr_t(out_ptr_t&& right) noexcept
		: Base(std::move(right)), m_smart_ptr(right.m_smart_ptr), m_old_ptr(right.m_old_ptr), m_target_ptr(right.m_target_ptr) {
			right.m_old_ptr == nullptr;
		}
		out_ptr_t& operator=(out_ptr_t&& right) noexcept {
			Base::operator	=(std::move(right));
			this->m_smart_ptr  = right.m_smart_ptr;
			this->m_old_ptr    = right.m_old_ptr;
			this->m_target_ptr = right.m_target_ptr;
			right.m_old_ptr == nullptr;
			return *this;
		}
		out_ptr_t(const out_ptr_t&) = delete;
		out_ptr_t& operator=(const out_ptr_t&) = delete;

		operator Pointer*() noexcept {
			return this->m_target_ptr;
		}
		operator Pointer&() noexcept {
			return *this->m_target_ptr;
		}

		~out_ptr_t() noexcept {
			reset();
		}

	private:
		void reset(boost::mp11::index_sequence<>) {
			if (this->m_old_ptr != nullptr) {
				this->m_smart_ptr->get_deleter()(static_cast<source_pointer>(this->m_old_ptr));
			}
		}

		template <std::size_t I0, std::size_t... I>
		void reset(boost::mp11::index_sequence<I0, I...>) {
			static_assert(always_false_index<I0>::value, "you cannot reset the deleter for handle<T, Deleter>!: it only takes one argument!");
		}
	};
} // namespace boost

int main() {
	phd::handle<int*, ficapi::int_deleter> p(nullptr);
	ficapi_create(boost::out_ptr<void*>(p), ficapi_type::ficapi_type_int);
	int* rawp = static_cast<int*>(p.get());
	assert(rawp != nullptr);
	assert(*rawp == ficapi_get_dynamic_data());
}