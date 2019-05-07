//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_CLEVER_OUT_PTR_HPP
#define BOOST_OUT_PTR_CLEVER_OUT_PTR_HPP

#include <boost/out_ptr/detail/clever_out_ptr_impl.hpp>
#include <boost/config.hpp>
#include <boost/assert.hpp>

#include <cstdlib>
#include <cstddef>
#include <type_traits>
#include <memory>
#include <exception>
#include <utility>
#include <tuple>

namespace boost {

	template <typename Smart, typename Pointer, typename... Args>
	class clever_out_ptr_t : public out_ptr_detail::clever_out_ptr_impl<Smart, Pointer, std::tuple<Args...>, std::make_index_sequence<std::tuple_size_v<std::tuple<Args...>>>> {
	private:
		using list_t = std::make_index_sequence<std::tuple_size_v<std::tuple<Args...>>>;
		using core_t = out_ptr_detail::clever_out_ptr_impl<Smart, Pointer, std::tuple<Args...>, list_t>;

	public:
		clever_out_ptr_t(Smart& s, Args... args)
		: core_t(s, std::forward_as_tuple(std::forward<Args>(args)...)) {
		}
	};

	template <typename Pointer,
		typename Smart,
		typename... Args>
	auto clever_out_ptr(Smart& p, Args&&... args) noexcept {
		using P = clever_out_ptr_t<Smart, Pointer, Args...>;
		return P(p, std::forward<Args>(args)...);
	}

	template <typename Smart,
		typename... Args>
	auto clever_out_ptr(Smart& p, Args&&... args) noexcept {
		using Pointer = pointer_of_t<Smart>;
		using P	  = clever_out_ptr_t<Smart, Pointer, Args...>;
		return P(p, std::forward<Args>(args)...);
	}

} // namespace boost

#endif // BOOST_OUT_PTR_CLEVER_OUT_PTR_HPP
