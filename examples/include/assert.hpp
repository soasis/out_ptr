//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#pragma once

#ifndef PHD_OUT_PTR_EXAMPLES_ASSERT
#define PHD_OUT_PTR_EXAMPLES_ASSERT

#ifndef NDEBUG

#include <iostream>
#include <cstdlib>

#define PHD_OUT_PTR_EXAMPLE_ASSERT_MSG(condition, message)                                                                                                                \
	(!(condition) ? ((void)(std::cerr << "Assertion `" #condition "` failed in " << __FILE__ << " line " << __LINE__ << ": " << message << std::endl), (void)std::abort()) \
			    : (void)0)

#define OUT_PTR_C_ASSERT(condition)                                                                                                                      \
	(!(condition) ? ((void)(std::cerr << "Assertion `" #condition "` failed in " << __FILE__ << " line " << __LINE__ << std::endl), (void)std::abort()) \
			    : (void)0)

#else

#define PHD_OUT_PTR_EXAMPLE_ASSERT_MSG(condition, message) \
	(false ? ((void)(condition), (void)(message)) : (void)0)

#define OUT_PTR_C_ASSERT(condition) \
	(false ? ((void)(condition)) : (void)0)

#endif // NDEBUG

#endif // PHD_OUT_PTR_EXAMPLES_ASSERT
