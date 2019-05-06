#pragma once

#ifndef BOOST_OUT_PTR_SIMPLE_INOUT_PTR_HPP
#define BOOST_OUT_PTR_SIMPLE_INOUT_PTR_HPP

#include <boost/out_ptr/detail/base_inout_ptr_impl.hpp>

#include <cstddef>
#include <type_traits>
#include <memory>
#include <utility>
#include <tuple>

namespace boost
{

template <typename Smart, typename Pointer, typename... Args>
class simple_inout_ptr_t : public out_ptr_detail::base_inout_ptr_impl<Smart, Pointer, std::tuple<Args...>, std::make_index_sequence<std::tuple_size_v<std::tuple<Args...>>>>
{
	using list_t = std::make_index_sequence<std::tuple_size_v<std::tuple<Args...>>>;
	using core_t = out_ptr_detail::base_inout_ptr_impl<Smart, Pointer, std::tuple<Args...>, list_t>;

public:
	simple_inout_ptr_t(Smart &s, Args... args)
	    : core_t(s, std::forward_as_tuple(std::forward<Args>(args)...))
	{
	}
};

template <typename Pointer,
		typename Smart,
		typename... Args>
auto simple_inout_ptr(Smart &p, Args &&... args) noexcept
{
	using P = simple_inout_ptr_t<Smart, Pointer, Args...>;
	return P(p, std::forward<Args>(args)...);
}

template <typename Smart,
		typename... Args>
auto simple_inout_ptr(Smart &p, Args &&... args) noexcept
{
	using Pointer = meta::pointer_of_t<Smart>;
	using P = simple_inout_ptr_t<Smart, Pointer, Args...>;
	return P(p, std::forward<Args>(args)...);
}

} // namespace boost

#endif // BOOST_OUT_PTR_SIMPLE_INOUT_PTR_HPP
