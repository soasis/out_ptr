//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_DETAIL_BASE_OUT_PTR_IMPL_HPP
#define BOOST_OUT_PTR_DETAIL_BASE_OUT_PTR_IMPL_HPP

#include <boost/config.hpp>
#include <boost/static_assert.hpp>
#include <boost/mp11/integer_sequence.hpp>
#include <boost/out_ptr/pointer_of.hpp>
#include <boost/out_ptr/detail/is_specialization_of.hpp>
#include <boost/out_ptr/detail/customization_forward.hpp>

#include <cstdlib>
#include <type_traits>
#include <memory>
#include <utility>
#include <tuple>

namespace boost {
namespace out_ptr_detail {

	template <typename Smart, typename... Args>
	void reset_or_create(std::true_type, Smart& s, Args&&... args) {
		s.reset(std::forward<Args>(args)...);
	}

	template <typename Smart, typename... Args>
	void reset_or_create(std::false_type, Smart& s, Args&&... args) {
		s = Smart(std::forward<Args>(args)...);
	}

	template <typename Smart, typename Pointer, typename Args, typename List>
	struct base_out_ptr_impl;

	template <typename Smart, typename Pointer, typename Base, std::size_t... Indices>
	struct base_out_ptr_impl<Smart, Pointer, Base, boost::mp11::index_sequence<Indices...>>
	: Base {
	protected:
		using source_pointer = pointer_of_or_t<Smart, Pointer>;

		Smart* m_smart_ptr;
		Pointer m_target_ptr;

		static_assert(!(out_ptr_detail::is_specialization_of<Smart, std::shared_ptr>::value || out_ptr_detail::is_specialization_of<Smart, ::boost::shared_ptr>::value)
				|| (sizeof...(Indices) > 0), // clang-format hack
			"shared_ptr<T> must pass a deleter in alongside the out_ptr "
			"so when reset is called the deleter can be properly "
			"initialized, otherwise the deleter will be defaulted "
			"by the shared_ptr<T>::reset() call!");

		base_out_ptr_impl(Smart& ptr, Base&& args, Pointer initial)
		: Base(std::move(args)), m_smart_ptr(std::addressof(ptr)), m_target_ptr(initial) {
		}

		base_out_ptr_impl(Smart& ptr, Base&& args, out_ptr_detail::disambiguate_)
		: Base(std::move(args)), m_smart_ptr(std::addressof(ptr)), m_target_ptr() {
		}

	public:
		base_out_ptr_impl(Smart& ptr, Base&& args)
		: base_out_ptr_impl(ptr, std::move(args), out_ptr_detail::disambiguate_()) {
		}

		base_out_ptr_impl(base_out_ptr_impl&& right)
		: Base(std::move(*this)), m_smart_ptr(right.m_smart_ptr), m_target_ptr(right.m_target_ptr) {
			right.m_smart_ptr = nullptr;
		}
		base_out_ptr_impl& operator=(base_out_ptr_impl&& right) {
			static_cast<Base&>(*this) = std::move(right);
			this->m_smart_ptr		 = right.m_smart_ptr;
			this->m_target_ptr		 = right.m_target_ptr;
			right.m_smart_ptr		 = nullptr;
			return *this;
		}
		base_out_ptr_impl(const base_out_ptr_impl&) = delete;
		base_out_ptr_impl& operator=(const base_out_ptr_impl&) = delete;

		operator Pointer*() noexcept {
			return std::addressof(this->m_target_ptr);
		}
		operator Pointer&() noexcept {
			return this->m_target_ptr;
		}

		~base_out_ptr_impl() {
			if (m_smart_ptr) {
				Base&& args = std::move(static_cast<Base&>(*this));
				// lmao "if constexpr" xD
				using can_reset = out_ptr_detail::is_resetable<Smart,
					decltype(static_cast<source_pointer>(this->m_target_ptr)),
					decltype(std::get<Indices>(std::move(args)))...>;
				reset_or_create(can_reset(), *this->m_smart_ptr, static_cast<source_pointer>(this->m_target_ptr), std::get<Indices>(std::move(args))...);
			}
		}
	};
}
} // namespace boost::out_ptr_detail
#endif // BOOST_OUT_PTR_DETAIL_BASE_OUT_PTR_IMPL_HPP
