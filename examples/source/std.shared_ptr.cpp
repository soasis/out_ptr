//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#include <phd/out_ptr.hpp>

#include <ficapi/ficapi.hpp>

#include <assert.hpp>

#include <memory>

int main(int, char* []) {

	std::shared_ptr<ficapi::opaque> p(nullptr);
	// next line would error: forgot to pass deleter and it got reset!
	//ficapi_handle_create(phd::out_ptr::out_ptr(p));

	// next line does not error
	ficapi_handle_create(phd::out_ptr::out_ptr(p, ficapi_handle_delete));
	ficapi::opaque_handle rawp = p.get();

	// avoid unused variable in release builds !
	OUT_PTR_C_ASSERT(rawp != nullptr);
	OUT_PTR_C_ASSERT(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());

	return 0;
}
