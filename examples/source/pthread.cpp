//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#include <phd/out_ptr.hpp>
#include <boost/smart_ptr.hpp>

#include <assert.hpp>

#include <iostream>

#include <pthread.h>

void* compute_result(void* args_ptr) {

	/* increment x to 100 */
	int& arg0 = *static_cast<int*>(args_ptr);

	// return the value from the thread
	int result = arg0 + arg0;
	return new int(result);
}

int main() {
	pthread_t compute_thread;

	int argument = 24;
	int expected = argument * 2;

	/* Launch a pthread to compute a value */
	if (pthread_create(&compute_thread, nullptr, compute_result, &argument)) {
		std::cerr << "Error creating thread" << std::endl;
		return 1;
	}

	// Grab the result returned by the pthread through its `void*`
	// and put it into the shared pointer
	std::unique_ptr<int> result(nullptr);
	auto err = pthread_join(compute_thread, phd::out_ptr::out_ptr<void*>(result));
	if (err) {
		std::cerr << "Error joining thread" << std::endl;
		return 2;
	}

	OUT_PTR_C_ASSERT(expected == *result);

	return 0;
}
