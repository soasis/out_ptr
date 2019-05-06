//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.


#include "handle.hpp"
#include <ficapi/ficapi.hpp>

#include <boost/out_ptr.hpp>

namespace boost {

	template <typename T, typename D, typename Pointer, typename... Args>
	class out_ptr_t<phd::handle<T, D>, Pointer, Args...> {
	public:
		using Smart		 = Handle;
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
			Args::operator	=(std::move(right));
			this->m_smart_ptr  = right.m_smart_ptr;
			this->m_old_ptr    = right.m_old_ptr;
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
} // namespace boost

int main() {
	boost::handle<int*, ficapi::int_deleter> p(nullptr);
	ficapi_create(boost::out_ptr(p), ficapi_type::ficapi_type_int);
	int* rawp = static_cast<int*>(p.get());
	assert(rawp != nullptr);
	assert(*rawp == ficapi_get_dynamic_data());
}