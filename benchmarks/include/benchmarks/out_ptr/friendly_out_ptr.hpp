//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef PHD_OUT_PTR_FRIENDLY_OUT_PTR_HPP
#define PHD_OUT_PTR_FRIENDLY_OUT_PTR_HPP

#if defined(PHD_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR) && PHD_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR != 0

#include <phd/out_ptr/detail/base_out_ptr_impl.hpp>
#include <phd/out_ptr/pointer_of.hpp>

#include <cstdlib>
#include <cstddef>
#include <type_traits>
#include <memory>
#include <tuple>

namespace phd { namespace out_ptr {

	namespace detail {
		template <typename Smart, typename Pointer, typename Args, typename List>
		class friendly_out_ptr_impl : public base_out_ptr_impl<Smart, Pointer, out_ptr_traits<Smart, Pointer>, Args, List> {
		private:
			using base_t = base_out_ptr_impl<Smart, Pointer, out_ptr_traits<Smart, Pointer>, Args, List>;

		public:
			using base_t::base_t;
		};
	} // namespace detail

	template <typename Smart, typename Pointer, typename... Args>
	struct friendly_out_ptr_t : public detail::friendly_out_ptr_impl<Smart, Pointer, std::tuple<Args...>, phd::out_ptr::detail::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>> {
	private:
		using list_t = phd::out_ptr::detail::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>;
		using core_t = detail::friendly_out_ptr_impl<Smart, Pointer, std::tuple<Args...>, list_t>;

	public:
		friendly_out_ptr_t(Smart& s, Args... args)
		: core_t(s, std::forward_as_tuple(std::forward<Args>(args)...)) {
		}
	};

	template <typename Pointer, typename Smart, typename... Args>
	friendly_out_ptr_t<Smart, Pointer, Args...> friendly_out_ptr(Smart& p, Args&&... args) noexcept {
		return friendly_out_ptr_t<Smart, Pointer, Args...>(p, std::forward<Args>(args)...);
	}

	template <typename Smart, typename... Args>
	friendly_out_ptr_t<Smart, pointer_of_t<Smart>, Args...> friendly_out_ptr(Smart& p, Args&&... args) noexcept {
		using Pointer = pointer_of_t<Smart>;
		return friendly_out_ptr_t<Smart, Pointer, Args...>(p, std::forward<Args>(args)...);
	}

}} // namespace phd::out_ptr

#endif // PHD_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR

#endif // PHD_OUT_PTR_FRIENDLY_OUT_PTR_HPP
