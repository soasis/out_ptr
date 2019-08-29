//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#define CATCH_CONFIG_RUNNER 1
#include <catch2/catch.hpp>

int main(int argc, char* argv[]) {
	int r = Catch::Session().run(argc, argv);
	return r;
}
