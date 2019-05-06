#pragma once

#ifndef BOOST_TESTS_C_API_HPP
#define BOOST_TESTS_C_API_HPP

struct base1 {
	virtual ~base1() {
	}
};
struct base2 {
	virtual ~base2() {
	}
};
struct derived1 : base1 {};
struct derived2 : base1, base2 {};

inline int init_derived2(derived2** pp_derived, bool please_fail = false) {
	if (please_fail) {
		// do not set *pp_derived
		// to test if internal
		// initialization of the out_ptr affects things
		return 1;
	}
	*pp_derived = new derived2();
	return 0;
}

#endif // BOOST_TESTS_C_API_HPP
