//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_DETAIL_IS_SPECIALIZATION_OF_HPP
#define BOOST_OUT_PTR_DETAIL_IS_SPECIALIZATION_OF_HPP

#include <type_traits>

namespace boost {
namespace out_ptr_detail {
	namespace meta_detail {
		template <typename T, template <typename...> class Templ>
		struct is_specialization_of : std::false_type {
		};
		template <typename... T, template <typename...> class Templ>
		struct is_specialization_of<Templ<T...>, Templ> : std::true_type {
		};

	} // namespace meta_detail

	template <typename T, template <typename...> class Templ>
	using is_specialization_of = meta_detail::is_specialization_of<typename std::remove_cv<T>::type, Templ>;
}
} // namespace boost::out_ptr_detail

#endif // BOOST_OUT_PTR_DETAIL_IS_SPECIALIZATION_OF_HPP