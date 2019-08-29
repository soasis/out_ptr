//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#pragma once

#ifndef PHD_OUT_PTR_HPP
#define PHD_OUT_PTR_HPP

#include <phd/out_ptr/detail/simple_out_ptr.hpp>
#include <phd/out_ptr/detail/clever_out_ptr.hpp>
#include <phd/out_ptr/detail/out_ptr_traits.hpp>
#include <phd/out_ptr/pointer_of.hpp>

#include <type_traits>
#include <tuple>

namespace phd { namespace out_ptr {

	namespace detail {

#if defined(PHD_OUT_PTR_USE_CLEVER_OUT_PTR) && PHD_OUT_PTR_USE_CLEVER_OUT_PTR != 0
		// One must opt into this specifically because it is unsafe for a large
		// variety of APIs
		template <typename Smart, typename Pointer, typename... Args>
		using core_out_ptr_t = clever_out_ptr_t<Smart, Pointer, Args...>;
#else
		// we can never use the clever version by default
		// because many C APIs do not set
		// the pointer to null on parameter failure
		template <typename Smart, typename Pointer, typename... Args>
		using core_out_ptr_t = simple_out_ptr_t<Smart, Pointer, Args...>;
#endif // PHD_OUT_PTR_USE_CLEVER_OUT_PTR

	} // namespace detail

	template <typename Smart, typename Pointer, typename... Args>
	class PHD_OUT_PTR_TRIVIAL_ABI out_ptr_t : public detail::core_out_ptr_t<Smart, Pointer, Args...> {
	private:
		using base_t = detail::core_out_ptr_t<Smart, Pointer, Args...>;

	public:
		using base_t::base_t;
	};

	template <typename Pointer, typename Smart, typename... Args>
	out_ptr_t<Smart, Pointer, Args...> out_ptr(Smart& s, Args&&... args) noexcept {
		using P = out_ptr_t<Smart, Pointer, Args...>;
		return P(s, std::forward<Args>(args)...);
	}

	template <typename Smart, typename... Args>
	out_ptr_t<Smart, pointer_of_t<Smart>, Args...> out_ptr(Smart& s, Args&&... args) noexcept {
		using Pointer = pointer_of_t<Smart>;
		using P	  = out_ptr_t<Smart, Pointer, Args...>;
		return P(s, std::forward<Args>(args)...);
	}

}} // namespace phd::out_ptr

#endif // PHD_OUT_PTR_HPP
