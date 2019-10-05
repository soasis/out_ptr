//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#pragma once

#ifndef PHD_OUT_PTR_NECESSARY_ARITY_HPP
#define PHD_OUT_PTR_NECESSARY_ARITY_HPP

#include <phd/out_ptr/detail/customization_forward.hpp>
#include <phd/out_ptr/detail/is_specialization_of.hpp>

#include <type_traits>
#include <memory>

namespace phd {
namespace out_ptr {
	namespace detail {
		template <typename Smart, typename... Args>
		class necessary_arity_impl : public std::integral_constant<std::size_t, static_cast<std::size_t>(is_specialization_of<Smart, std::shared_ptr>::value || is_specialization_of<Smart, boost::shared_ptr>::value)> {};
	} // namespace detail

	template <typename Smart, typename... Args>
	class necessary_arity : public detail::necessary_arity_impl<Smart, Args...> {};

	template <typename Smart, typename... Args>
#if __cplusplus >= 201703LL
	inline static constexpr std::size_t necessary_arity_v = necessary_arity<Smart, Args...>::value;
#else
	static constexpr std::size_t necessary_arity_v = necessary_arity<Smart, Args...>::value;
#endif
}
} // namespace phd::out_ptr

#endif // PHD_OUT_PTR_NECESSARY_ARITY_HPP
