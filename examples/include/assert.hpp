// Copyright ⓒ 2018-2021 ThePhD.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#pragma once

#ifndef ZTD_OUT_PTR_EXAMPLES_ASSERT
#define ZTD_OUT_PTR_EXAMPLES_ASSERT

#ifndef NDEBUG

#include <iostream>
#include <cstdlib>

#define ZTD_OUT_PTR_EXAMPLE_ASSERT_MSG(condition, message)                                                                                                                \
	(!(condition) ? ((void)(std::cerr << "Assertion `" #condition "` failed in " << __FILE__ << " line " << __LINE__ << ": " << message << std::endl), (void)std::abort()) \
			    : (void)0)

#define OUT_PTR_C_ASSERT(condition)                                                                                                                      \
	(!(condition) ? ((void)(std::cerr << "Assertion `" #condition "` failed in " << __FILE__ << " line " << __LINE__ << std::endl), (void)std::abort()) \
			    : (void)0)

#else

#define ZTD_OUT_PTR_EXAMPLE_ASSERT_MSG(condition, message) \
	(false ? ((void)(condition), (void)(message)) : (void)0)

#define OUT_PTR_C_ASSERT(condition) \
	(false ? ((void)(condition)) : (void)0)

#endif // NDEBUG

#endif // ZTD_OUT_PTR_EXAMPLES_ASSERT
