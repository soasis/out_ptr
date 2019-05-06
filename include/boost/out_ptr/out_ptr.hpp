#pragma once

#ifndef BOOST_OUT_PTR_HPP
#define BOOST_OUT_PTR_HPP

#include <boost/out_ptr/simple_out_ptr.hpp>
#include <boost/out_ptr/clever_out_ptr.hpp>
#include <boost/config.hpp>
#include <boost/assert.hpp>

#include <type_traits>
#include <tuple>

namespace boost
{

namespace detail
{
#if defined(BOOST_OUT_PTR_NO_CLEVERNESS) && BOOST_OUT_PTR_NO_CLEVERNESS != 0
template <typename Smart, typename Pointer, typename... Args>
using core_out_ptr_t = simple_out_ptr_t<Smart, Pointer, Args...>;
#else
template <typename Smart, typename Pointer, typename... Args>
using core_out_ptr_t = clever_out_ptr_t<Smart, Pointer, Args...>;
#endif // BOOST_OUT_PTR_NO_CLEVERNESS
} // namespace detail

template <typename Smart, typename Pointer, typename... Args>
class out_ptr_t : public detail::core_out_ptr_t<Smart, Pointer, Args...>
{
private:
	using base_t = detail::core_out_ptr_t<Smart, Pointer, Args...>;

public:
	using base_t::base_t;
};

template <typename Pointer,
		typename Smart,
		typename... Args>
auto out_ptr(Smart &s, Args &&... args) noexcept
{
	using P = out_ptr_t<Smart, Pointer, Args...>;
	return P(s, std::forward<Args>(args)...);
}

template <typename Smart,
		typename... Args>
auto out_ptr(Smart &s, Args &&... args) noexcept
{
	using Pointer = meta::pointer_of_t<Smart>;
	using P = out_ptr_t<Smart, Pointer, Args...>;
	return P(s, std::forward<Args>(args)...);
}

} // namespace boost

#endif // BOOST_OUT_PTR_HPP
