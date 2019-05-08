//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#include <boost/out_ptr.hpp>

#include <ficapi/ficapi.hpp>

#include <memory>
#include <cassert>

int main(int, char* []) {

	std::shared_ptr<ficapi::opaque> p(nullptr);
	// next line would error: forgot to pass deleter and it got reset!
	//ficapi_handle_create(boost::out_ptr(p));

	// next line does not error
	ficapi_handle_create(boost::out_ptr(p, ficapi_handle_delete));
	ficapi::opaque_handle rawp = p.get();

	assert(rawp != nullptr);
	assert(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());

	return 0;
}
