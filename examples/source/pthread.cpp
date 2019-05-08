//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#include <boost/out_ptr.hpp>
#include <boost/smart_ptr.hpp>

#include <iostream>
#include <cassert>

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
	boost::shared_ptr<int> result(nullptr);
	if (pthread_join(compute_thread, boost::out_ptr<void*>(result, std::default_delete<int>()))) {
		std::cerr << "Error joining thread" << std::endl;
		return 2;
	}

	assert(expected == *result);

	return 0;
}
