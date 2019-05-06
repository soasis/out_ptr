#pragma once

#ifndef BOOST_OUT_PTR_INOUT_PTR_HPP
#define BOOST_OUT_PTR_INOUT_PTR_HPP

#include <boost/out_ptr/clever_inout_ptr.hpp>
#include <boost/out_ptr/simple_inout_ptr.hpp>
#include <boost/version/version.hpp>

namespace boost
{

namespace detail
{
#if defined(BOOST_OUT_PTR_NO_CLEVERNESS) && BOOST_OUT_PTR_NO_CLEVERNESS != 0
template <typename Smart, typename Pointer, typename... Args>
using core_inout_ptr_t = simple_inout_ptr_t<Smart, Pointer, Args...>;
#else
template <typename Smart, typename Pointer, typename... Args>
using core_inout_ptr_t = clever_inout_ptr_t<Smart, Pointer, Args...>;
#endif // BOOST_OUT_PTR_NO_CLEVERNESS
} // namespace detail

template <typename Smart, typename Pointer, typename... Args>
class inout_ptr_t : public detail::core_inout_ptr_t<Smart, Pointer, Args...>
{
private:
	using base_t = detail::core_inout_ptr_t<Smart, Pointer, Args...>;

public:
	using base_t::base_t;
};

template <typename Pointer,
		typename Smart,
		typename... Args>
auto inout_ptr(Smart &s, Args &&... args) noexcept
{
	using P = inout_ptr_t<Smart, Pointer, Args...>;
	return P(s, std::forward<Args>(args)...);
}

template <typename Smart,
		typename... Args>
auto inout_ptr(Smart &s, Args &&... args) noexcept
{
	using Pointer = meta::pointer_of_t<Smart>;
	using P = inout_ptr_t<Smart, Pointer, Args...>;
	return P(s, std::forward<Args>(args)...);
}

} // namespace boost

#endif // BOOST_OUT_PTR_INOUT_PTR_HPP
