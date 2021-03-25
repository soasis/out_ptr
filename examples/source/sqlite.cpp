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

// Thanks to David Sankel for coming up with this example

#include <ztd/out_ptr.hpp>

#include <sqlite3.h>

#include <memory>
#include <iostream>

int main(int, char* []) {
	using zop = ztd::out_ptr;

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
