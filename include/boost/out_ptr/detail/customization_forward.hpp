//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_DETAIL_CUSTOMIZATION_FORWARD_HPP
#define BOOST_OUT_PTR_DETAIL_CUSTOMIZATION_FORWARD_HPP

namespace boost {

	namespace movelib {
		template <typename, typename>
		class unique_ptr;
	}

	template <typename>
	class shared_ptr;

} // namespace boost

#endif // BOOST_OUT_PTR_DETAIL_CUSTOMIZATION_FORWARD_HPP