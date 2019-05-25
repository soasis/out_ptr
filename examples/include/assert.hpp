//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_EXAMPLES_ASSERT
#define BOOST_OUT_PTR_EXAMPLES_ASSERT

#ifndef NDEBUG

#include <iostream>
#include <cstdlib>

#define BOOST_OUT_PTR_EXAMPLE_ASSERT_MSG(condition, message)                                                                                                                \
	(!(condition) ? ((void)(std::cerr << "Assertion `" #condition "` failed in " << __FILE__ << " line " << __LINE__ << ": " << message << std::endl), (void)std::abort()) \
			    : (void)0)

#define OUT_PTR_C_ASSERT(condition)                                                                                                                      \
	(!(condition) ? ((void)(std::cerr << "Assertion `" #condition "` failed in " << __FILE__ << " line " << __LINE__ << std::endl), (void)std::abort()) \
			    : (void)0)

#else

#define BOOST_OUT_PTR_EXAMPLE_ASSERT_MSG(condition, message) \
	(false ? ((void)(condition), (void)(message)) : (void)0)

#define OUT_PTR_C_ASSERT(condition) \
	(false ? ((void)(condition)) : (void)0)

#endif // NDEBUG

#endif // BOOST_OUT_PTR_EXAMPLES_ASSERT
