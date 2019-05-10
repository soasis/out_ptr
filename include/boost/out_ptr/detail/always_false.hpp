//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_DETAIL_ALWAYS_FALSE
#define BOOST_OUT_PTR_DETAIL_ALWAYS_FALSE

#include <type_traits>

namespace boost {
namespace out_ptr {
namespace detail {

	template <typename>
	using always_false = std::false_type;

	template <std::size_t>
	using always_false_index = std::false_type;

}}} // namespace boost::out_ptr::detail

#endif // BOOST_OUT_PTR_DETAIL_ALWAYS_FALSE