//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#include <boost/out_ptr/out_ptr.hpp>

#include <iostream>
#include <fstream>
#include <array>
#include <cstdio>
#include <cstddef>

#ifdef _WIN32

#else

// Some functions to achieve cross-platform parity

#include <assert.h>

int fopen_s(FILE** f, const char* name, const char* mode) {
	auto ret = 0;
	assert(f);
	*f = std::fopen(name, mode);
	/* Can't be sure about 1-to-1 mapping of errno and MS' errno_t */
	if (!*f)
		ret = errno;
	return ret;
}

#endif // Windows

struct fclose_deleter {
	void operator()(FILE* f) const {
		std::fclose(f);
	}
};

int main() {
	constexpr auto file_name = "out_ptr.example.file";
	{
		// write file to test with our example
		std::ofstream example_file(file_name);
		example_file << "hello, everyone!" << std::flush;
	}

	{
		std::unique_ptr<FILE, fclose_deleter> my_unique_fptr;

		// open file, stuff it in this deleter
		auto err = fopen_s(boost::out_ptr::out_ptr<FILE*>(my_unique_fptr), file_name, "r+b");
		// check err, then work with fd
		if (err != 0) {
			// something horrible happened
			std::cerr << "Error is non-zero, could not open " << file_name << " with a raw handle." << std::endl;
		}
		else {
			char file_data[51]{};
			std::cout << "Opened " << file_name << " sucessfully! Reading up to 50 bytes..." << std::endl;
			std::size_t file_data_count = std::fread(file_data, 1, 50, my_unique_fptr.get());
			std::cout << "Successfully read " << file_data_count << " bytes! They read:\n"
					<< file_data << std::endl;
		}
	}

	{
		// delete file we just created
		std::remove(file_name);
	}
	return 0;
}
