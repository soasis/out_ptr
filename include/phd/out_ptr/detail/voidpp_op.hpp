//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef PHD_OUT_PTR_DETAIL_VOIDPP_OP_HPP
#define PHD_OUT_PTR_DETAIL_VOIDPP_OP_HPP

namespace phd { namespace out_ptr { namespace detail {

	template <typename T, typename P>
	class voidpp_op {
	public:
		operator void**() const noexcept(noexcept(static_cast<P*>(std::declval<T&>()))) {
			const T& self = *static_cast<T const*>(this);
			return static_cast<void**>(static_cast<void*>(static_cast<P*>(self)));
		};
	};

	template <typename T>
	class voidpp_op<T, void*> {};

	template <typename T>
	class voidpp_op<T, const void*> {};

	template <typename T>
	class voidpp_op<T, const volatile void*> {};

}}} // namespace phd::out_ptr::detail

#endif // PHD_OUT_PTR_DETAIL_VOIDPP_OP_HPP
