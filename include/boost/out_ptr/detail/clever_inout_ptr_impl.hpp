//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_DETAIL_CLEVER_INOUT_PTR_IMPL_HPP
#define BOOST_OUT_PTR_DETAIL_CLEVER_INOUT_PTR_IMPL_HPP

#include <boost/out_ptr/detail/base_inout_ptr_impl.hpp>
#include <boost/out_ptr/detail/voidpp_op.hpp>
#include <boost/meta/is_specialization_of.hpp>
#include <boost/meta/pointer_of.hpp>

#include <memory>
#include <tuple>
#include <utility>

namespace boost::out_ptr_detail
{

template <typename Smart, typename Pointer, typename Args, typename List, typename = void>
struct clever_inout_ptr_impl : public base_inout_ptr_impl<Smart, Pointer, Args, List>
{
private:
	using base_t = base_inout_ptr_impl<Smart, Pointer, Args, List>;

public:
	using base_t::base_t;
};

template <typename T, typename D, typename Pointer>
struct clever_inout_ptr_impl<std::unique_ptr<T, D>, Pointer, std::tuple<>, std::index_sequence<>,
					    std::enable_if_t<
						   std::is_same_v<meta::pointer_of_t<std::unique_ptr<T, D>>, Pointer> || std::is_base_of_v<meta::pointer_of_t<std::unique_ptr<T, D>>, Pointer> || !std::is_convertible_v<meta::pointer_of_t<std::unique_ptr<T, D>>, Pointer>>>
    : voidpp_op<clever_inout_ptr_impl<std::unique_ptr<T, D>, Pointer, std::tuple<>, std::index_sequence<>>, Pointer>
{
public:
	typedef std::unique_ptr<T, D> Smart;
	typedef meta::pointer_of_or_t<Smart, Pointer> source_pointer;

private:
	using can_aliasing_optimization = std::integral_constant<bool,
												  sizeof(std::unique_ptr<T, D>) <= sizeof(Pointer) && sizeof(std::unique_ptr<T, D>) <= sizeof(source_pointer)>;

	Pointer *m_target_ptr;

	clever_inout_ptr_impl(std::true_type, Smart &ptr) noexcept
	    : m_target_ptr(static_cast<Pointer *>(static_cast<void *>(std::addressof(ptr))))
	{
		// we can assume things are cleaner here
#if defined(BOOST_OUT_PTR_CLEVER_SANITY_CHECK) && BOOST_OUT_PTR_CLEVER_SANITY_CHECK != 0
		if (*this->m_target_ptr != static_cast<Pointer>(ptr.get()))
		{
			std::terminate();
		}
#endif // Clever Sanity Checks
	}

	clever_inout_ptr_impl(std::false_type, Smart &ptr) noexcept
	{
		// analysis necessary
#if defined(BOOST_OUT_PTR_CLEVER_UNIQUE_IMPLEMENTATION_FIRST_MEMBER) && BOOST_OUT_PTR_CLEVER_UNIQUE_IMPLEMENTATION_FIRST_MEMBER != 0
		// implementation has Pointer as first member: alias directly
		void *target = static_cast<void *>(ptr);
#else
		// implementation has Pointer as second member: shift, align, alias
		constexpr const std::size_t memory_start = sizeof(D) + (sizeof(D) % alignof(D));
		std::size_t max_space = sizeof(Smart) - memory_start;
		void *source = static_cast<void *>(static_cast<char *>(static_cast<void *>(std::addressof(ptr))) + memory_start);
		void *target = std::align(alignof(source_pointer), sizeof(source_pointer), source, max_space);
#endif
		// get direct Pointer
		this->m_target_ptr = static_cast<Pointer *>(target);
#if defined(BOOST_OUT_PTR_CLEVER_SANITY_CHECK) && BOOST_OUT_PTR_CLEVER_SANITY_CHECK != 0
		if (*this->m_target_ptr != static_cast<Pointer>(ptr.get()))
		{
			std::terminate();
		}
#endif // Clever Sanity Checks
	}

public:
	clever_inout_ptr_impl(Smart &ptr, std::tuple<> &&) noexcept
	    : clever_inout_ptr_impl(can_aliasing_optimization(), ptr)
	{
	}
	clever_inout_ptr_impl(clever_inout_ptr_impl &&right) noexcept = default;
	clever_inout_ptr_impl &operator=(clever_inout_ptr_impl &&right) noexcept = default;
	clever_inout_ptr_impl(const clever_inout_ptr_impl &) = delete;
	clever_inout_ptr_impl &operator=(const clever_inout_ptr_impl &) = delete;

	operator Pointer *() noexcept
	{
		return this->m_target_ptr;
	}
	operator Pointer &() noexcept
	{
		return *this->m_target_ptr;
	}
	// no need for destructor: aliasing directly, and realloc
};
} // namespace boost::out_ptr_detail

#endif // BOOST_OUT_PTR_DETAIL_CLEVER_INOUT_PTR_IMPL_HPP
