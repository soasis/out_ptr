//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_SIMPLE_INOUT_PTR_HPP
#define BOOST_OUT_PTR_SIMPLE_INOUT_PTR_HPP

#include <boost/out_ptr/detail/base_inout_ptr_impl.hpp>

#include <cstddef>
#include <type_traits>
#include <memory>
#include <utility>
#include <tuple>

namespace boost {
namespace ptr {
namespace out_ptr_detail {

	template <typename Smart, typename Pointer, typename... Args>
	class simple_inout_ptr_t : public base_inout_ptr_impl<Smart, Pointer, std::tuple<Args...>, boost::mp11::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>> {
		using list_t = boost::mp11::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>;
		using core_t = base_inout_ptr_impl<Smart, Pointer, std::tuple<Args...>, list_t>;

	public:
		simple_inout_ptr_t(Smart& s, Args... args)
		: core_t(s, std::forward_as_tuple(std::forward<Args>(args)...)) {
		}
	};

	template <typename Pointer, typename Smart, typename... Args>
	simple_inout_ptr_t<Smart, Pointer, Args...> simple_inout_ptr(Smart& p, Args&&... args) noexcept {
		using P = simple_inout_ptr_t<Smart, Pointer, Args...>;
		return P(p, std::forward<Args>(args)...);
	}

	template <typename Smart, typename... Args>
	simple_inout_ptr_t<Smart, pointer_of_t<Smart>, Args...> simple_inout_ptr(Smart& p, Args&&... args) noexcept {
		using Pointer = pointer_of_t<Smart>;
		using P	  = simple_inout_ptr_t<Smart, Pointer, Args...>;
		return P(p, std::forward<Args>(args)...);
	}

}}} // namespace boost::ptr::out_ptr_detail

#endif // BOOST_OUT_PTR_SIMPLE_INOUT_PTR_HPP
