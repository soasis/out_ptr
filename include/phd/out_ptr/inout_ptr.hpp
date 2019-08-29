//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#pragma once

#ifndef PHD_OUT_PTR_INOUT_PTR_HPP
#define PHD_OUT_PTR_INOUT_PTR_HPP

#include <cstddef>

#if !defined(PHD_OUT_PTR_USE_CLEVER_INOUT_PTR)
#if defined(_LIBCPP_VERSION) || defined(__GLIBC__) || defined(_YVALS) || defined(_CPPLIB_VER)
#define PHD_OUT_PTR_USE_CLEVER_INOUT_PTR 1
#endif // Checked with VC++ lib/Dinkumware, libstdc++, and libc++
#endif // To use the clever optimization or not

#include <phd/out_ptr/detail/clever_inout_ptr.hpp>
#include <phd/out_ptr/detail/simple_inout_ptr.hpp>
#include <phd/out_ptr/detail/inout_ptr_traits.hpp>

namespace phd {
namespace out_ptr {

	namespace detail {
#if (defined(PHD_OUT_PTR_USE_CLEVER_INOUT_PTR) && PHD_OUT_PTR_USE_CLEVER_INOUT_PTR != 0)
		template <typename Smart, typename Pointer, typename... Args>
		using core_inout_ptr_t = clever_inout_ptr_t<Smart, Pointer, Args...>;
#else
		template <typename Smart, typename Pointer, typename... Args>
		using core_inout_ptr_t = simple_inout_ptr_t<Smart, Pointer, Args...>;
#endif // PHD_OUT_PTR_USE_CLEVER_INOUT_PTR

	} // namespace detail

	template <typename Smart, typename Pointer, typename... Args>
	class PHD_OUT_PTR_TRIVIAL_ABI inout_ptr_t : public detail::core_inout_ptr_t<Smart, Pointer, Args...> {
	private:
		using base_t = detail::core_inout_ptr_t<Smart, Pointer, Args...>;

	public:
		using base_t::base_t;
	};

	template <typename Pointer, typename Smart, typename... Args>
	inout_ptr_t<Smart, Pointer, Args...> inout_ptr(Smart& s, Args&&... args) noexcept {
		using P = inout_ptr_t<Smart, Pointer, Args...>;
		return P(s, std::forward<Args>(args)...);
	}

	template <typename Smart, typename... Args>
	inout_ptr_t<Smart, pointer_of_t<Smart>, Args...> inout_ptr(Smart& s, Args&&... args) noexcept {
		using Pointer = pointer_of_t<Smart>;
		using P	  = inout_ptr_t<Smart, Pointer, Args...>;
		return P(s, std::forward<Args>(args)...);
	}

}} // namespace phd::out_ptr

#endif // PHD_OUT_PTR_INOUT_PTR_HPP
