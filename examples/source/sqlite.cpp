//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

// Thanks to David Sankel for coming up with this example

#include <phd/out_ptr.hpp>

#include <sqlite3.h>

#include <memory>
#include <iostream>

int main(int, char* []) {
	using pop = phd::out_ptr;

	std::shared_ptr<sqlite3> database;

	if (SQLITE_OK != sqlite3_open("test.db", pop::out_ptr(database, sqlite3_close))) {
		std::cout << "Problem opening the file";
	}

	using Sqlite3Ptr = std::unique_ptr<sqlite3, decltype(&sqlite3_close)>;
	Sqlite3Ptr database2(nullptr, &sqlite3_close);
	if (SQLITE_OK != sqlite3_open("test.db", pop::out_ptr(database2, sqlite3_close))) {
		std::cout << "Problem opening the file";
	}

	return 0;
}
