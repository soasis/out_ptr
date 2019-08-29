//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#pragma once

#ifndef PHD_OUT_PTR_SIMPLE_OUT_PTR_HPP
#define PHD_OUT_PTR_SIMPLE_OUT_PTR_HPP

#include <phd/out_ptr/detail/base_out_ptr_impl.hpp>

#include <cstddef>
#include <type_traits>
#include <memory>
#include <utility>
#include <tuple>

namespace phd {
namespace out_ptr {
namespace detail {

	template <typename Smart, typename Pointer, typename... Args>
	class PHD_OUT_PTR_TRIVIAL_ABI simple_out_ptr_t : public base_out_ptr_impl<Smart, Pointer, out_ptr_traits<Smart, Pointer>, std::tuple<Args...>, phd::out_ptr::detail::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>> {
	private:
		using list_t = phd::out_ptr::detail::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>;
		using core_t = base_out_ptr_impl<Smart, Pointer, out_ptr_traits<Smart, Pointer>, std::tuple<Args...>, list_t>;

	public:
		simple_out_ptr_t(Smart& s, Args... args) noexcept
		: core_t(s, std::forward_as_tuple(std::forward<Args>(args)...)) {
		}
	};

	template <typename Pointer,
		typename Smart,
		typename... Args>
	simple_out_ptr_t<Smart, Pointer, Args...> simple_out_ptr(Smart& p, Args&&... args) noexcept {
		using P = simple_out_ptr_t<Smart, Pointer, Args...>;
		return P(p, std::forward<Args>(args)...);
	}

	template <typename Smart, typename... Args>
	simple_out_ptr_t<Smart, pointer_of_t<Smart>, Args...> simple_out_ptr(Smart& p, Args&&... args) noexcept {
		using Pointer = pointer_of_t<Smart>;
		using P	  = simple_out_ptr_t<Smart, Pointer, Args...>;
		return P(p, std::forward<Args>(args)...);
	}

}}} // namespace phd::out_ptr::detail

#endif // PHD_OUT_PTR_SIMPLE_OUT_PTR_HPP
