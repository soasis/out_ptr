//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#include <phd/out_ptr.hpp>
#include <boost/smart_ptr/local_shared_ptr.hpp>

#include <avformat.h>

// a shared pointer
// that we need to make sure is
// not used with inout_ptr
// because ownership is not unique or releasable!
template <typename T>
struct my_company_shared_ptr : public phd::local_shared_ptr<T> {};

// type to allow us to use static assert
// that is not eagerly evaluated
template <typename>
struct dependent_type_false : std::true_type {};

// this is the usage of the simple traits customization point
// to achieve the same goals as above

namespace phd { namespace out_ptr {

	// inout_ptr_t is illegal
	template <typename T, typename Pointer>
	class inout_ptr_traits<my_company_shared_ptr<T>, Pointer> {
		static_assert(dependent_type_false<T>::value,
			"you cannot release from a shared pointer "
			"there is no way to release the resource from all "
			"the shared pointers!");
	};

	// require all out_ptr calls to need
	// a deleter argument to make it
	// safer to use
	template <typename T, typename Pointer>
	class out_ptr_traits<my_company_shared_ptr<T>, Pointer> {
		template <typename... Args>
		Pointer construct(Smart&, Args&&...) {
			static_assert(sizeof...(Args) > 0, "you forgot to pass the deleter that is supposed to come along with this type!");
			return Pointer{};
		}

		template <typename... Args>
		void reset(Smart& s, Pointer& pointer, Args&&... args) {
			static_assert(sizeof...(Args) > 0, "you forgot to pass the deleter that is supposed to come along with this type!");
			s.reset(pointer, std::forward<Args>(args)...);
		}
	};

}} // namespace phd::out_ptr

struct av_format_context_deleter {
	void operator()(AVFormatContext* c) noexcept {
		avformat_close_input(&c);
		avformat_free_context(c);
	}
};

using av_format_context_ptr = my_company_shared_ptr<AVFormatContext>;

int main(int, char* argv[]) {
	using bop = phd::out_ptr;

	av_format_context_ptr context(avformat_alloc_context());

	// cannot use it with inout_ptr
	//if (0 != avformat_open_input(bop::out_ptr(raw_context, av_format_context_deleter{}), argv[0], nullptr, nullptr)) { ... }

	// cannot use it wihout a deleter
	//if (0 != avformat_open_input(bop::out_ptr(raw_context, av_format_context_deleter{}), argv[0], nullptr, nullptr)) { ... }

	// this is the only way to use it properly!
	if (0 != avformat_open_input(bop::out_ptr(raw_context, av_format_context_deleter{}), argv[0], nullptr, nullptr)) {
		std::stringstream ss;
		ss << "ffmpeg_image_loader could not open file '"
		   << path << "'";
		throw FFmpegInputException(ss.str().c_str());
	}

	return 0;
}
