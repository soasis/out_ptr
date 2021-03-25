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

#include <ztd/out_ptr/out_ptr.hpp>

#include <assert.hpp>

#include <iostream>
#include <array>
#include <cstdio>
#include <cstddef>

#ifdef _WIN32

#include <io.h>
#include <fcntl.h>

#else

// Some functions to achieve cross-platform parity

#include <unistd.h>

int fopen_s(FILE** f, const char* name, const char* mode) {
	int ret = 0;
	OUT_PTR_C_ASSERT(f);
	*f = fopen(name, mode);
	/* Can't be sure about 1-to-1 mapping of errno and MS' errno_t */
	if (!*f)
		ret = errno;
	return ret;
}

#endif // Windows

struct fd {
	int handle;

	fd()
	: fd(nullptr) {
	}
	fd(std::nullptr_t)
	: handle(static_cast<intptr_t>(-1)) {
	}
	fd(FILE* f)
#ifdef _WIN32
	: handle(f ? _fileno(f) : static_cast<intptr_t>(-1)){
#else
	: handle(f ? fileno(f) : static_cast<intptr_t>(-1)) {
#endif // Windows
	}

	  operator FILE*() const {
		if (!*this)
			return nullptr;
#ifdef _WIN32
		return _fdopen(handle, "rb");
#else
		return fdopen(handle, "rb");
#endif // Windows
	}

	explicit operator bool() const {
		return *this != nullptr;
	}

	bool operator==(std::nullptr_t) const {
		return handle == -1;
	}

	bool operator!=(std::nullptr_t) const {
		return handle != -1;
	}

	bool operator==(const fd& fd) const {
		return handle == fd.handle;
	}

	bool operator!=(const fd& fd) const {
		return handle != fd.handle;
	}

	bool operator==(int fdhandle) const {
		return handle == fdhandle;
	}

	bool operator!=(int fdhandle) const {
		return handle != fdhandle;
	}
};

struct fd_deleter {
	using pointer = fd;

	void operator()(fd des) const {
		if (des.handle != -1) {
#ifdef _WIN32
			_close(des.handle);
#else
			close(des.handle);
#endif // Windows
		}
	}
};

int main() {
	std::unique_ptr<int, fd_deleter> my_unique_fd;

	auto err = fopen_s(ztd::out_ptr::out_ptr<FILE*>(my_unique_fd), "prod.csv", "r+b");
	// check err, then work with fd
	if (err != 0) {
		// something horrible happened, handle etc. etc.
		std::cout << "Error is non-zero, could not open prod.csv with a raw handle." << std::endl;
	}
	else {
		std::cout << "Opened prod.csv with a raw handle!" << std::endl;
	}
	return 0;
}
