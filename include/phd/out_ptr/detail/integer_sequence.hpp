//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef PHD_OUT_PTR_DETAIL_INTEGER_SEQUENCE_HPP
#define PHD_OUT_PTR_DETAIL_INTEGER_SEQUENCE_HPP

#if __cplusplus > 201400L
#include <utility>
#endif // Non-shit standards only
#include <cstddef>

namespace phd { namespace out_ptr { namespace detail {

#if __cplusplus > 201400L

	template <typename T, T... Values>
	using integer_sequence = std::integer_sequence<T, Values...>;

	template <std::size_t... Values>
	using index_sequence = std::index_sequence<Values...>;

	template<typename T, T N>
	using make_integer_sequence = std::make_integer_sequence<T, N>;
			
	template<std::size_t N>
	using make_index_sequence = std::make_index_sequence<N>;

#else

	template <typename T, T... Values>
	class integer_sequence {};

	template <std::size_t... Values>
	using index_sequence = integer_sequence<std::size_t, Values...>;

	namespace impl {
		template <typename T, T N, bool, T... Values>
		struct mis {
			using type = typename mis<T, N - 1, ((N - 1) == 0), N - 1, Values...>::type;
		};
		
		template <typename T, T N, T... Values>
		struct mis<T, N, true, Values...> {
			using type = integer_sequence<T, Values...>;
		};
	}

	template<typename T, T N>
	using make_integer_sequence = typename impl::mis<T, N, N == 0>::type;
			
	template<std::size_t N>
	using make_index_sequence = make_integer_sequence<std::size_t, N>;

#endif

}}}

#endif // PHD_OUT_PTR_DETAIL_INTEGER_SEQUENCE_HPP
