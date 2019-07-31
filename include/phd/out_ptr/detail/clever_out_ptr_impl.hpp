//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef PHD_OUT_PTR_DETAIL_CLEVER_OUT_PTR_IMPL_HPP
#define PHD_OUT_PTR_DETAIL_CLEVER_OUT_PTR_IMPL_HPP

#include <phd/out_ptr/detail/base_out_ptr_impl.hpp>
#include <phd/out_ptr/detail/customization_forward.hpp>
#include <phd/out_ptr/detail/voidpp_op.hpp>
#include <phd/out_ptr/detail/integer_sequence.hpp>

#include <memory>
#include <cassert>

#if defined(_LIBCPP_VERSION)
// Why is their __compressed_pair <pointer, deleter>
// and not <deleter, pointer>? It still seems to optimize the bases correctly,
// so maybe this is more so my own hubris as a libstdc++ baby
#if !defined(PHD_OUT_PTR_CLEVER_UNIQUE_IMPLEMENTATION_FIRST_MEMBER)
#define PHD_OUT_PTR_CLEVER_UNIQUE_IMPLEMENTATION_FIRST_MEMBER 1
#endif // std::unique_ptr

#endif // Libc++ does pointer first...!

namespace phd {
namespace out_ptr {
namespace detail {

	template <typename Smart, typename T, typename D, typename Pointer>
	class PHD_OUT_PTR_TRIVIAL_ABI out_unique_fast : voidpp_op<out_unique_fast<Smart, T, D, Pointer>, Pointer> {
	protected:
		using source_pointer = pointer_of_or_t<Smart, Pointer>;

	private:
		using can_aliasing_optimization = std::integral_constant<bool,
			sizeof(Smart) <= sizeof(Pointer) && sizeof(Smart) <= sizeof(source_pointer)>;
		Smart* m_smart_ptr;
		source_pointer m_old_ptr;
		Pointer* m_target_ptr;

		out_unique_fast(std::true_type, Smart& ptr) noexcept
		: m_smart_ptr(std::addressof(ptr)), m_old_ptr(ptr.get()), m_target_ptr(static_cast<Pointer*>(static_cast<void*>(this->m_smart_ptr))) {
			// we can assume things are cleaner here
#if defined(PHD_OUT_PTR_CLEVER_SANITY_CHECK) && PHD_OUT_PTR_CLEVER_SANITY_CHECK != 0
			assert(*this->m_target_ptr == this->m_old_ptr && "clever UB-based optimization did not properly retrieve the pointer value");
#endif // Clever Sanity Checks
		}

		out_unique_fast(std::false_type, Smart& ptr) noexcept
		: m_smart_ptr(std::addressof(ptr)), m_old_ptr(static_cast<Pointer>(ptr.get())) {
			// analysis necessary
			if (is_specialization_of<Smart, phd::movelib::unique_ptr>::value) {
#if defined(PHD_OUT_PTR_CLEVER_UNIQUE_MOVELIB_IMPLEMENTATION_FIRST_MEMBER) && PHD_OUT_PTR_CLEVER_MOVELIB_UNIQUE_IMPLEMENTATION_FIRST_MEMBER != 0
				// implementation has Pointer as first member: alias directly
				void* target = static_cast<void*>(this->m_smart_ptr);
#else
				// implementation has Pointer as second member: shift, align, alias
				constexpr const std::size_t memory_start = std::is_empty<D>::value ? 0 : sizeof(D) + (sizeof(D) % alignof(D));
				std::size_t max_space				 = sizeof(Smart) - memory_start;
				void* source						 = static_cast<void*>(static_cast<char*>(static_cast<void*>(this->m_smart_ptr)) + memory_start);
				void* target						 = std::align(alignof(source_pointer), sizeof(source_pointer), source, max_space);
#endif
				// get direct Pointer
				this->m_target_ptr = static_cast<Pointer*>(target);
			}
			else {
#if defined(PHD_OUT_PTR_CLEVER_UNIQUE_IMPLEMENTATION_FIRST_MEMBER) && PHD_OUT_PTR_CLEVER_UNIQUE_IMPLEMENTATION_FIRST_MEMBER != 0
				// implementation has Pointer as first member: alias directly
				void* target = static_cast<void*>(this->m_smart_ptr);
#else
				// implementation has Pointer as second member: shift, align, alias
				constexpr const std::size_t memory_start = std::is_empty<D>::value ? 0 : sizeof(D) + (sizeof(D) % alignof(D));
				std::size_t max_space				 = sizeof(Smart) - memory_start;
				void* source						 = static_cast<void*>(static_cast<char*>(static_cast<void*>(this->m_smart_ptr)) + memory_start);
				void* target						 = std::align(alignof(source_pointer), sizeof(source_pointer), source, max_space);
#endif
				// get direct Pointer
				this->m_target_ptr = static_cast<Pointer*>(target);
			}
#if defined(PHD_OUT_PTR_CLEVER_SANITY_CHECK) && PHD_OUT_PTR_CLEVER_SANITY_CHECK != 0
			assert(*this->m_target_ptr == this->m_old_ptr && "clever UB-based optimization did not properly retrieve the pointer value");
#endif // Clever Sanity Checks
		}

	public:
		out_unique_fast(Smart& ptr, std::tuple<>&&) noexcept
		: out_unique_fast(can_aliasing_optimization(), ptr) {
		}
		out_unique_fast(out_unique_fast&& right) noexcept
		: m_smart_ptr(right.m_smart_ptr), m_old_ptr(right.m_old_ptr), m_target_ptr(right.m_target_ptr) {
			right.m_old_ptr = nullptr;
		}
		out_unique_fast& operator=(out_unique_fast&& right) noexcept {
			this->m_smart_ptr  = right.m_smart_ptr;
			this->m_old_ptr    = right.m_old_ptr;
			this->m_target_ptr = right.m_target_ptr;
			right.m_old_ptr    = nullptr;
			return *this;
		}

		operator Pointer*() const noexcept {
			return const_cast<Pointer*>(this->m_target_ptr);
		}

		~out_unique_fast() noexcept {
			if (this->m_old_ptr != nullptr) {
				this->m_smart_ptr->get_deleter()(static_cast<source_pointer>(this->m_old_ptr));
			}
		}
	};

	template <typename Smart, typename Pointer, typename Args, typename List, typename = void>
	class PHD_OUT_PTR_TRIVIAL_ABI clever_out_ptr_impl : public base_out_ptr_impl<Smart, Pointer, out_ptr_traits<Smart, Pointer>, Args, List> {
	private:
		using base_t = base_out_ptr_impl<Smart, Pointer, out_ptr_traits<Smart, Pointer>, Args, List>;

	public:
		using base_t::base_t;
	};

	template <typename T, typename D, typename Pointer>
	class PHD_OUT_PTR_TRIVIAL_ABI clever_out_ptr_impl<std::unique_ptr<T, D>,
		Pointer, std::tuple<>, phd::out_ptr::detail::index_sequence<>,
		typename std::enable_if<std::is_same<pointer_of_or_t<std::unique_ptr<T, D>, Pointer>, Pointer>::value
			|| detail::has_unspecialized_marker<out_ptr_traits<std::unique_ptr<T, D>, Pointer>>::value
			|| std::is_base_of<pointer_of_or_t<std::unique_ptr<T, D>, Pointer>, Pointer>::value
			|| !std::is_convertible<pointer_of_or_t<std::unique_ptr<T, D>, Pointer>, Pointer>::value>::type>
	: public out_unique_fast<std::unique_ptr<T, D>, T, D, Pointer> {
	private:
		using base_t = out_unique_fast<std::unique_ptr<T, D>, T, D, Pointer>;

	public:
		using base_t::base_t;
	};

	template <typename T, typename D, typename Pointer>
	struct clever_out_ptr_impl<phd::movelib::unique_ptr<T, D>,
		Pointer, std::tuple<>, phd::out_ptr::detail::index_sequence<>,
		typename std::enable_if<std::is_same<pointer_of_or_t<phd::movelib::unique_ptr<T, D>, Pointer>, Pointer>::value
			|| detail::has_unspecialized_marker<out_ptr_traits<phd::movelib::unique_ptr<T, D>, Pointer>>::value
			|| std::is_base_of<pointer_of_or_t<phd::movelib::unique_ptr<T, D>, Pointer>, Pointer>::value
			|| !std::is_convertible<pointer_of_or_t<phd::movelib::unique_ptr<T, D>, Pointer>, Pointer>::value>::type>
	: public out_unique_fast<phd::movelib::unique_ptr<T, D>, T, D, Pointer> {
	private:
		using base_t = out_unique_fast<phd::movelib::unique_ptr<T, D>, T, D, Pointer>;

	public:
		using base_t::base_t;
	};
}}} // namespace phd::out_ptr::detail

#endif // PHD_OUT_PTR_DETAIL_CLEVER_OUT_PTR_IMPL_HPP
