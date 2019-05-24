//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_DETAIL_ALWAYS_FALSE
#define BOOST_OUT_PTR_DETAIL_ALWAYS_FALSE

#include <type_traits>
#include <cstddef>

namespace boost {
namespace out_ptr {
namespace detail {

	// NOTE:
	// if these are using aliases and not,
	// base structs, clang in C++11 mode
	// sees right through them and performs no SFINAE
	// or substitution, meaning
	// it will error in static_asserts that take
	// the template argument since it can see
	// the template argument is not used through
	// the using
	// which is kind of shitty, but that's how it goes!

	template <typename>
	struct always_false : std::false_type {};

	template <std::size_t>
	struct always_false_index : std::false_type {};

}}} // namespace boost::out_ptr::detail

#endif // BOOST_OUT_PTR_DETAIL_ALWAYS_FALSE