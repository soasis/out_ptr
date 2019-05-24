//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_FRIENDLY_INOUT_PTR_HPP
#define BOOST_OUT_PTR_FRIENDLY_INOUT_PTR_HPP

#if defined(BOOST_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR) && BOOST_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR != 0

#include <boost/out_ptr/detail/base_inout_ptr_impl.hpp>
#include <boost/out_ptr/pointer_of.hpp>

#include <cstdlib>
#include <cstddef>
#include <type_traits>
#include <memory>

namespace boost { namespace out_ptr {

	namespace detail {
		template <typename Smart, typename Pointer, typename Args, typename List>
		struct friendly_inout_ptr_impl : public base_inout_ptr_impl<Smart, Pointer, Args, List> {
		private:
			using base_t = base_inout_ptr_impl<Smart, Pointer, Args, List>;

		public:
			using base_t::base_t;
		};

		template <typename T, typename D, typename Pointer>
		struct friendly_inout_ptr_impl<std::friendly_unique_ptr<T, D>, Pointer, std::tuple<>, boost::mp11::index_sequence<>> {
		public:
			using Smart		 = std::friendly_unique_ptr<T, D>;
			using source_pointer = pointer_of_or_t<Smart, Pointer>;

		private:
			Pointer* m_target_ptr;

		public:
			friendly_inout_ptr_impl(Smart& ptr, std::tuple<>) noexcept
			: m_target_ptr(static_cast<Pointer*>(static_cast<void*>(&ptr.friendly_get()))) {
			}

			friendly_inout_ptr_impl(friendly_inout_ptr_impl&& right) noexcept = default;
			friendly_inout_ptr_impl& operator=(friendly_inout_ptr_impl&& right) noexcept = default;
			friendly_inout_ptr_impl(const friendly_inout_ptr_impl&)				  = delete;
			friendly_inout_ptr_impl& operator=(const friendly_inout_ptr_impl&) = delete;

			operator Pointer*() const noexcept {
				return const_cast<Pointer*>(m_target_ptr);
			}
		};
	} // namespace detail

	template <typename Smart, typename Pointer, typename... Args>
	class friendly_inout_ptr_t : public detail::friendly_inout_ptr_impl<Smart, Pointer, std::tuple<Args...>, boost::mp11::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>> {
	private:
		using list_t = boost::mp11::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>;
		using core_t = detail::friendly_inout_ptr_impl<Smart, Pointer, std::tuple<Args...>, list_t>;

	public:
		friendly_inout_ptr_t(Smart& s, Args... args)
		: core_t(s, std::forward_as_tuple(std::forward<Args>(args)...)) {
		}
	};

	template <typename Pointer, typename Smart, typename... Args>
	friendly_inout_ptr_t<Smart, Pointer, Args...> friendly_inout_ptr(Smart& p, Args&&... args) noexcept {
		return friendly_inout_ptr_t<Smart, Pointer, Args...>(p, std::forward<Args>(args)...);
	}

	template <typename Smart, typename... Args>
	friendly_inout_ptr_t<Smart, pointer_of_t<Smart>, Args...> friendly_inout_ptr(Smart& p, Args&&... args) noexcept {
		using Pointer = pointer_of_t<Smart>;
		return friendly_inout_ptr<Pointer>(p, std::forward<Args>(args)...);
	}

}} // namespace boost::out_ptr

#endif // OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR

#endif // BOOST_OUT_PTR_FRIENDLY_INOUT_PTR_HPP
