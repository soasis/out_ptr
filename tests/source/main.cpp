// Copyright ⓒ 2018-2023 ThePhD.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#define CATCH_CONFIG_RUNNER 1
#include <catch2/catch.hpp>

#include <ztd/out_ptr.hpp>

#include <iostream>

int main(int argc, char* argv[]) {
	std::cout << "ZTD_OUT_PTR_CLEVER_SANITY_CHECK_I_ = " << ZTD_OUT_PTR_CLEVER_SANITY_CHECK_I_ << std::endl;
	std::cout << "ZTD_OUT_PTR_USE_CLEVER_INOUT_PTR_I_ = " << ZTD_OUT_PTR_USE_CLEVER_INOUT_PTR_I_ << std::endl;
	std::cout << "ZTD_OUT_PTR_CLEVER_UNIQUE_IMPLEMENTATION_FIRST_MEMBER_I_ = " << ZTD_OUT_PTR_CLEVER_UNIQUE_IMPLEMENTATION_FIRST_MEMBER_I_ << std::endl;
	std::cout << "ZTD_OUT_PTR_CLEVER_MOVELIB_UNIQUE_IMPLEMENTATION_FIRST_MEMBER_I_ = " << ZTD_OUT_PTR_CLEVER_MOVELIB_UNIQUE_IMPLEMENTATION_FIRST_MEMBER_I_ << std::endl;

	int r = Catch::Session().run(argc, argv);
	return r;
}
