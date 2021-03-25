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

#include <ztd/out_ptr.hpp>

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
	auto err = pthread_join(compute_thread, ztd::out_ptr::out_ptr<void*>(result));
	if (err) {
		std::cerr << "Error joining thread" << std::endl;
		return 2;
	}

	OUT_PTR_C_ASSERT(expected == *result);

	return 0;
}
