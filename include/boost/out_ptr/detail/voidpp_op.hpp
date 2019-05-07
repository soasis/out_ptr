//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_DETAIL_VOIDPP_OP_HPP
#define BOOST_OUT_PTR_DETAIL_VOIDPP_OP_HPP

namespace boost {
namespace out_ptr_detail {

	template <typename T, typename P>
	struct voidpp_op {
		operator void*&() noexcept(noexcept(static_cast<P*>(std::declval<T&>()))) {
			T& self = *static_cast<T*>(static_cast<void*>(this));
			return *static_cast<void**>(static_cast<void*>(static_cast<P*>(self)));
		};
		operator void**() noexcept(noexcept(static_cast<P*>(std::declval<T&>()))) {
			T& self = *static_cast<T*>(static_cast<void*>(this));
			return static_cast<void**>(static_cast<void*>(static_cast<P*>(self)));
		};
	};

	template <typename T>
	struct voidpp_op<T, void*> {
	};

	template <typename T>
	struct voidpp_op<T, const void*> {
	};

	template <typename T>
	struct voidpp_op<T, const volatile void*> {
	};

}} // namespace boost::out_ptr_detail

#endif // BOOST_OUT_PTR_DETAIL_VOIDPP_OP_HPP
