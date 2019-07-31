//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_FRIENDLY_OUT_PTR_HPP
#define BOOST_OUT_PTR_FRIENDLY_OUT_PTR_HPP

#if defined(BOOST_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR) && BOOST_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR != 0

#include <boost/out_ptr/detail/base_out_ptr_impl.hpp>
#include <boost/out_ptr/pointer_of.hpp>

#include <cstdlib>
#include <cstddef>
#include <type_traits>
#include <memory>
#include <tuple>

namespace boost { namespace out_ptr {

	namespace detail {
		template <typename Smart, typename Pointer, typename Args, typename List>
		class friendly_out_ptr_impl : public base_out_ptr_impl<Smart, Pointer, out_ptr_traits<Smart, Pointer>, Args, List> {
		private:
			using base_t = base_out_ptr_impl<Smart, Pointer, out_ptr_traits<Smart, Pointer>, Args, List>;

		public:
			using base_t::base_t;
		};

#if 0
		// We cannot rely on this since it will double-free in cases where the pointers isn't maintained
		// our benchmarks have a pointer that goes in nullptr all the time,
		// so there's a use case for this that enables the speed,
		// but in the general case the optimization can't be turned on...!
		template <typename T, typename D, typename Pointer>
		struct friendly_out_ptr_impl<std::friendly_unique_ptr<T, D>, Pointer, std::tuple<>, boost::mp11::index_sequence<>> {
		public:
			using Smart		 = std::friendly_unique_ptr<T, D>;
			using source_pointer = pointer_of_or_t<Smart, Pointer>;

		private:
			Smart* m_smart_ptr;
			Pointer m_old_ptr;
			Pointer* m_target_ptr;

		public:
			friendly_out_ptr_impl(Smart& ptr, std::tuple<>&&) noexcept
			: m_smart_ptr(std::addressof(ptr)), m_old_ptr(static_cast<Pointer>(ptr.get())), m_target_ptr(static_cast<Pointer*>(static_cast<void*>(std::addressof(ptr.friendly_get())))) {
			}

			friendly_out_ptr_impl(friendly_out_ptr_impl&& right) noexcept
			: m_smart_ptr(right.m_smart_ptr), m_old_ptr(right.m_old_ptr), m_target_ptr(right.m_target_ptr) {
				right.m_old_ptr = nullptr;
			}
			friendly_out_ptr_impl& operator=(friendly_out_ptr_impl&& right) noexcept {
				m_smart_ptr	= right.m_smart_ptr;
				m_old_ptr		 = right.m_old_ptr;
				m_target_ptr    = right.m_target_ptr;
				right.m_old_ptr = nullptr;
				return *this;
			}
			friendly_out_ptr_impl(const friendly_out_ptr_impl&) = delete;
			friendly_out_ptr_impl& operator=(const friendly_out_ptr_impl&) = delete;

			operator Pointer*() const noexcept {
				return const_cast<Pointer*>(m_target_ptr);
			}

			~friendly_out_ptr_impl() noexcept {
				if (m_old_ptr != nullptr) {
					m_smart_ptr->get_deleter()(m_old_ptr);
				}
			}
		};
#endif // no way to optimized out_ptr, but inout_ptr is still game

	} // namespace detail

	template <typename Smart, typename Pointer, typename... Args>
	struct friendly_out_ptr_t : public detail::friendly_out_ptr_impl<Smart, Pointer, std::tuple<Args...>, boost::mp11::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>> {
	private:
		using list_t = boost::mp11::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>;
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

}} // namespace boost::out_ptr

#endif // BOOST_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR

#endif // BOOST_OUT_PTR_FRIENDLY_OUT_PTR_HPP
