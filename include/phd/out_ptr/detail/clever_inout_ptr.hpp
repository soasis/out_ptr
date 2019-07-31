//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef PHD_OUT_PTR_CLEVER_INOUT_PTR_HPP
#define PHD_OUT_PTR_CLEVER_INOUT_PTR_HPP

#include <phd/out_ptr/detail/clever_inout_ptr_impl.hpp>

#include <cstdlib>
#include <cstddef>
#include <type_traits>
#include <memory>
#include <exception>
#include <utility>
#include <tuple>

namespace phd {
namespace out_ptr {
namespace detail {

	template <typename Smart, typename Pointer, typename... Args>
	class PHD_OUT_PTR_TRIVIAL_ABI clever_inout_ptr_t : public clever_inout_ptr_impl<Smart, Pointer, std::tuple<Args...>, phd::out_ptr::detail::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>> {
	private:
		using list_t = phd::out_ptr::detail::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>;
		using core_t = clever_inout_ptr_impl<Smart, Pointer, std::tuple<Args...>, list_t>;

	public:
		clever_inout_ptr_t(Smart& s, Args... args) noexcept
		: core_t(s, std::forward_as_tuple(std::forward<Args>(args)...)) {
		}
	};

	template <typename Pointer, typename Smart, typename... Args>
	clever_inout_ptr_t<Smart, Pointer, Args...> clever_inout_ptr(Smart& p, Args&&... args) noexcept {
		using P = clever_inout_ptr_t<Smart, Pointer, Args...>;
		return P(p, std::forward<Args>(args)...);
	}

	template <typename Smart, typename... Args>
	clever_inout_ptr_t<Smart, pointer_of_t<Smart>, Args...> clever_inout_ptr(Smart& p, Args&&... args) noexcept {
		using Pointer = pointer_of_t<Smart>;
		using P	  = clever_inout_ptr_t<Smart, Pointer, Args...>;
		return P(p, std::forward<Args>(args)...);
	}

}}} // namespace phd::out_ptr::detail

#endif // PHD_OUT_PTR_CLEVER_INOUT_PTR_HPP
