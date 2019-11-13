//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#pragma once

#ifndef PHD_OUT_PTR_DETAIL_BASE_OUT_PTR_IMPL_HPP
#define PHD_OUT_PTR_DETAIL_BASE_OUT_PTR_IMPL_HPP

#include <phd/out_ptr/necessary_arity.hpp>
#include <phd/out_ptr/detail/is_specialization_of.hpp>
#include <phd/out_ptr/detail/customization_forward.hpp>
#include <phd/out_ptr/detail/inout_ptr_traits.hpp>
#include <phd/out_ptr/detail/voidpp_op.hpp>
#include <phd/out_ptr/pointer_of.hpp>
#include <phd/out_ptr/detail/integer_sequence.hpp>

#include <cstdlib>
#include <type_traits>
#include <memory>
#include <utility>
#include <tuple>

// Only defined for clang version 7 and above
#if defined(__clang__) && __clang__ >= 7
#define PHD_OUT_PTR_TRIVIAL_ABI __attribute__((trivial_abi))
#else
#define PHD_OUT_PTR_TRIVIAL_ABI
#endif // Clang or otherwise

namespace phd {
namespace out_ptr {
namespace detail {

	template <typename Smart, typename Pointer, typename Traits, typename Args, typename List>
	class PHD_OUT_PTR_TRIVIAL_ABI base_out_ptr_impl;

	template <typename Smart, typename Pointer, typename Traits, typename Base, std::size_t... Indices>
	class PHD_OUT_PTR_TRIVIAL_ABI base_out_ptr_impl<Smart, Pointer, Traits, Base, phd::out_ptr::detail::index_sequence<Indices...>>
	: public voidpp_op<base_out_ptr_impl<Smart, Pointer, Traits, Base, phd::out_ptr::detail::index_sequence<Indices...>>, Pointer>, protected Base {
	protected:
		using traits_t = Traits;
		using storage  = pointer_of_or_t<traits_t, Pointer>;
		Smart* m_smart_ptr;
		storage m_target_ptr;

		static_assert(sizeof...(Indices) >= necessary_arity<Smart, typename std::tuple_element<Indices, Base>::type...>::value, // clang-format hack
			"out_ptr requires certain arguments to be passed in for use with this type "
			"(e.g. shared_ptr<T> must pass a deleter in so when reset is called the "
			"deleter can be properly initialized, otherwise the deleter will be "
			"defaulted by the shared_ptr<T>::reset() call!)");

		base_out_ptr_impl(Smart& ptr, Base&& args, storage initial) noexcept
		: Base(std::move(args)), m_smart_ptr(std::addressof(ptr)), m_target_ptr(initial) {
		}

	public:
		base_out_ptr_impl(Smart& ptr, Base&& args) noexcept
		: Base(std::move(args)), m_smart_ptr(std::addressof(ptr)), m_target_ptr(traits_t::construct(ptr, std::get<Indices>(static_cast<Base&>(*this))...)) {
		}

		base_out_ptr_impl(base_out_ptr_impl&& right) noexcept
		: Base(std::move(right)), m_smart_ptr(right.m_smart_ptr), m_target_ptr(std::move(right.m_target_ptr)) {
			right.m_smart_ptr = nullptr;
		}
		base_out_ptr_impl& operator=(base_out_ptr_impl&& right) noexcept {
			static_cast<Base&>(*this) = std::move(right);
			this->m_smart_ptr		 = std::move(right.m_smart_ptr);
			this->m_target_ptr		 = std::move(right.m_target_ptr);
			right.m_smart_ptr		 = nullptr;
			return *this;
		}

		operator Pointer*() const noexcept {
			using has_get_call = std::integral_constant<bool, has_traits_get_call<traits_t>::value>;
			return call_traits_get<traits_t>(has_get_call(), *const_cast<Smart*>(this->m_smart_ptr), const_cast<storage&>(this->m_target_ptr));
		}

		~base_out_ptr_impl() noexcept(noexcept(traits_t::reset(std::declval<Smart&>(), std::declval<storage&>(), std::get<Indices>(std::move(std::declval<Base&>()))...))) {
			if (this->m_smart_ptr) {
				Base&& args = std::move(static_cast<Base&>(*this));
				(void)args; // unused if "Indices" is empty
				traits_t::reset(*this->m_smart_ptr, this->m_target_ptr, std::get<Indices>(std::move(args))...);
			}
		}
	};

}}} // namespace phd::out_ptr::detail

#endif // PHD_OUT_PTR_DETAIL_BASE_OUT_PTR_IMPL_HPP
