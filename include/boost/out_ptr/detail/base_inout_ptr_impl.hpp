//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_DETAIL_BASE_INOUT_PTR_IMPL_HPP
#define BOOST_OUT_PTR_DETAIL_BASE_INOUT_PTR_IMPL_HPP

#include <boost/out_ptr/detail/base_out_ptr_impl.hpp>

#include <cstdlib>
#include <type_traits>
#include <memory>
#include <utility>

namespace boost {
namespace out_ptr {
namespace detail {

	template <typename Smart>
	void call_release(std::true_type, Smart& s) noexcept(noexcept(std::declval<Smart&>().release())) {
		s.release();
	}

	template <typename Smart>
	void call_release(std::false_type, Smart&) noexcept {
		static_assert(std::is_pointer<Smart>::value, "the type that does not have release called on it must be a pointer type");
	}

	template <typename Smart, typename Pointer, typename Args, typename List>
	struct base_inout_ptr_impl : base_out_ptr_impl<Smart, Pointer, Args, List> {
	private:
		using base_t = base_out_ptr_impl<Smart, Pointer, Args, List>;

	public:
		base_inout_ptr_impl(Smart& ptr, Args&& args) noexcept
		: base_t(ptr, std::move(args), static_cast<Pointer>(ptr.get())) {
			static_assert(is_releasable<Smart>::value, "You cannot use an inout pointer with something that cannot release() its pointer!");
		}

		base_inout_ptr_impl(base_inout_ptr_impl&& right) noexcept
		: base_t(std::move(right)) {
		}
		base_inout_ptr_impl& operator=(base_inout_ptr_impl&& right) noexcept {
			static_cast<base_t&>(*this) = std::move(right);
		}

		base_inout_ptr_impl(const base_inout_ptr_impl&) = delete;
		base_inout_ptr_impl& operator=(const base_inout_ptr_impl&) = delete;

		~base_inout_ptr_impl() noexcept(std::is_nothrow_destructible<base_t>::value&& noexcept(call_release(is_releasable<Smart>(), std::declval<Smart&>()))) {
			call_release(is_releasable<Smart>(), *(this->m_smart_ptr));
		}
	};

}}} // namespace boost::out_ptr::detail

#endif // BOOST_OUT_PTR_DETAIL_BASE_INOUT_PTR_IMPL_HPP
