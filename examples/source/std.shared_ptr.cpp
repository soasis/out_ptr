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

#include <ficapi/ficapi.hpp>

#include <assert.hpp>

#include <memory>

int main(int, char* []) {

	std::shared_ptr<ficapi::opaque> p(nullptr);
	// next line would error: forgot to pass deleter and it got reset!
	//ficapi_handle_create(ztd::out_ptr::out_ptr(p));

	// next line does not error
	ficapi_handle_create(ztd::out_ptr::out_ptr(p, ficapi_handle_delete));
	ficapi::opaque_handle rawp = p.get();

	// avoid unused variable in release builds !
	OUT_PTR_C_ASSERT(rawp != nullptr);
	OUT_PTR_C_ASSERT(ficapi_handle_get_data(rawp) == ficapi_get_dynamic_data());

	return 0;
}
