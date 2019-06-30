//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_DETAIL_INOUT_PTR_TRAITS_HPP
#define BOOST_OUT_PTR_DETAIL_INOUT_PTR_TRAITS_HPP

#include <boost/out_ptr/detail/out_ptr_traits.hpp>
#include <boost/out_ptr/pointer_of.hpp>

#include <type_traits>

namespace boost {
namespace out_ptr {

	namespace detail {
		template <typename Smart>
		void call_release(std::true_type, Smart& s) noexcept(noexcept(std::declval<Smart&>().release())) {
			s.release();
		}

		template <typename Smart>
		void call_release(std::false_type, Smart&) noexcept {
			static_assert(std::is_pointer<Smart>::value, "the type that does not have release called on it must be a pointer type");
		}
	} // namespace detail

	template <typename Smart, typename Pointer>
	class inout_ptr_traits {
	private:
		using OUT_PTR_DETAIL_UNSPECIALIZED_MARKER_ = int;
		using defer_t						   = out_ptr_traits<Smart, Pointer>;

	public:
		using pointer = Pointer;

		template <typename... Args>
		static pointer construct(Smart& s, Args&&...) noexcept {
			return static_cast<pointer>(s.get());
		}

		static typename std::add_pointer<pointer>::type get(Smart& s, pointer& p) noexcept {
			return std::addressof(p);
		}

		template <typename... Args>
		static void reset(Smart& s, pointer p, Args&&... args) noexcept {
			detail::call_release(detail::is_releasable<Smart>(), s);
			defer_t::reset(s, std::move(p), std::forward<Args>(args)...);
		}
	};

}} // namespace boost::out_ptr

#endif // BOOST_OUT_PTR_DETAIL_INOUT_PTR_TRAITS_HPP
