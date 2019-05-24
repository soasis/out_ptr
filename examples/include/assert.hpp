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

#ifdef NDEBUG

#define OUT_PTR_ASSERT_MSG(condition, message)                                    \
	do {                                                                         \
		if (!(condition)) {                                                     \
			std::cerr << "Assertion `" #condition "` failed in " << __FILE__   \
					<< " line " << __LINE__ << ": " << message << std::endl; \
			std::terminate();                                                  \
		}                                                                       \
	} while (false)

#define OUT_PTR_C_ASSERT(condition)                                             \
	do {                                                                       \
		if (!(condition)) {                                                   \
			std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
					<< " line " << __LINE__ << std::endl;                  \
			std::terminate();                                                \
		}                                                                     \
	} while (false)

#else

#define OUT_PTR_ASSERT_MSG(condition, message) \
	do {                                      \
		if (false) {                         \
			(void)(condition);              \
			(void)sizeof(message);          \
		}                                    \
	} while (false)

#define OUT_PTR_C_ASSERT(condition) \
	do {                           \
		if (false) {              \
			(void)(condition);   \
		}                         \
	} while (false)

#endif // NDEBUG

#endif // BOOST_OUT_PTR_EXAMPLES_ASSERT
