// Copyright � 2018 ThePhD

#include <phd/out_ptr/out_ptr.hpp>

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
#include <assert.h>

auto fopen_s(FILE** f, const char* name, const char* mode) {
	auto ret = 0;
	assert(f);
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

	auto err = fopen_s(phd::out_ptr<FILE*>(my_unique_fd), "prod.csv", "r+b");
	// check err, then work with fd
	if (err != 0) {
		// something horrible happened
		std::cout << "Error is non-zero, could not open prod.csv with a raw handle." << std::endl;
	}
	else {
		std::cout << "Opened prod.csv with a raw handle!" << std::endl;
	}
	return 0;
}
