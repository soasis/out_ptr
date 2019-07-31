//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef PHD_OUT_PTR_TESTS_C_API_HPP
#define PHD_OUT_PTR_TESTS_C_API_HPP

#include <cstddef>
#include <tuple>

namespace phd { namespace out_ptr { namespace test {

	struct base1 {
		int a    = 0;
		size_t b = 1;
		virtual ~base1() {
		}
	};

	struct base2 {
		int c    = 3;
		double d = 4;

		virtual ~base2() {
		}
	};

	struct derived1 : base1 {
		char e = '\x04';

		bool operator==(const derived1& right) const {
			return std::tie(a, b, e) == std::tie(right.a, right.b, right.e);
		}
	};

	struct derived2 : base1, base2 {
		int f    = 5;
		double g = 6.5;

		bool operator==(const derived2& right) const {
			return std::tie(a, b, c, d, f, g) == std::tie(right.a, right.b, right.c, right.d, right.f, right.g);
		}
	};

	inline int init_derived2(derived2** pp_derived, bool please_fail = false) {
		if (please_fail) {
			return 1;
		}
		*pp_derived = new derived2();
		return 0;
	}

	inline int init_derived1(derived1** pp_derived, bool please_fail = false) {
		if (please_fail) {
			return 1;
		}
		*pp_derived = new derived1();
		return 0;
	}

	inline int erased_init_derived1(void** vpp_derived, bool please_fail = false) {
		derived1** pp_derived = static_cast<derived1**>(static_cast<void*>(vpp_derived));
		return init_derived1(pp_derived, please_fail);
	}

	inline int erased_init_derived2(void** vpp_derived, bool please_fail = false) {
		derived2** pp_derived = static_cast<derived2**>(static_cast<void*>(vpp_derived));
		return init_derived2(pp_derived, please_fail);
	}

	inline bool check_derived2(derived2& target) {
		return target == derived2{};
	}

	inline bool check_derived1(derived1& target) {
		return target == derived1{};
	}

}}} // namespace phd::out_ptr::test

#endif // PHD_OUT_PTR_TESTS_C_API_HPP
