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
#include <boost/core/empty_value.hpp>
#include <boost/smart_ptr/local_shared_ptr.hpp>

#include <avformat.h>

// a shared pointer
// that we need to make sure is
// not used with inout_ptr
// because ownership is not unique or releasable!
template <typename T>
struct my_company_shared_ptr : public ztd::local_shared_ptr<T> {};

// type to allow us to use static assert
// that is not eagerly evaluated
template <typename>
struct dependent_type_false : std::true_type {};

// this customization point is the more powerful version
namespace ztd { namespace out_ptr {

	template <typename T, typename Pointer, typename... Args>
	class inout_ptr_t<my_company_shared_ptr<T>, Pointer, Args...> {
		static_assert(dependent_type_false<T>::value,
			"you cannot release from a shared pointer "
			"there is no way to release the resource from all "
			"the shared pointers!");
	};

	// require all out_ptr calls to need
	// a deleter argument to make it
	// safer to use
	template <typename T, typename D, typename Pointer, typename... Args>
	class out_ptr_t<my_company_shared_ptr<T, D>, Pointer, Args...> : std::tuple<Args...> {
	private:
		using Smart		 = my_company_shared_ptr<T>;
		using source_pointer = pointer_of_or_t<Smart, Pointer>;
		using ArgsTuple	 = std::tuple<Args...>;
		using Base		 = ArgsTuple;

		static_assert(sizeof...(Args) > 0, "you forgot to pass a deleter: it will (most likely) be reset to the wrong one!");

		Smart* m_smart_ptr;
		Pointer m_target_ptr;

	public:
		out_ptr_t(Smart& s, Args... args) noexcept
		: Base(std::forward<Args>(args)...), m_smart_ptr(std::addressof(s)), m_old_ptr(s.get()), m_target_ptr() {
		}

		out_ptr_t(out_ptr_t&& right) noexcept
		: Base(std::move(right)), m_smart_ptr(right.m_smart_ptr), m_old_ptr(right.m_old_ptr), m_target_ptr(right.m_target_ptr) {
			right.m_old_ptr = nullptr;
		}
		out_ptr_t& operator=(out_ptr_t&& right) noexcept {
			Base::operator	=(std::move(right));
			this->m_smart_ptr  = std::move(right.m_smart_ptr);
			this->m_target_ptr = std::move(right.m_target_ptr);
			right.m_smart_ptr  = nullptr;
			return *this;
		}

		operator Pointer*() const noexcept {
			return const_cast<Pointer*>(this->m_target_ptr);
		}

		~out_ptr_t() noexcept {
			reset(ztd::out_ptr::op_detail::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>());
		}

	private:
		template <std::size_t I0, std::size_t... I>
		void reset(ztd::out_ptr::op_detail::index_sequence<I0, I...>) {
			if (this->m_smart_ptr != nullptr) {
				Base&& base = static_cast<Base&&>(*this);
				this->m_smart_ptr->reset(static_cast<source_pointer>(this->m_target_ptr, std::get<I0>(std::move(base)), std::get<I>(std::move(base))...));
			}
		}
	};

}} // namespace ztd::out_ptr

struct av_format_context_deleter {
	void operator()(AVFormatContext* c) noexcept {
		avformat_close_input(&c);
		avformat_free_context(c);
	}
};

using av_format_context_ptr = my_company_shared_ptr<AVFormatContext>;

int main(int, char* argv[]) {
	using zop = ztd::out_ptr;

	av_format_context_ptr context(avformat_alloc_context());

	// cannot use it with inout_ptr
	//if (0 != avformat_open_input(pop::out_ptr(raw_context, av_format_context_deleter{}), argv[0], nullptr, nullptr)) { ... }

	// cannot use it wihout a deleter
	//if (0 != avformat_open_input(pop::out_ptr(raw_context, av_format_context_deleter{}), argv[0], nullptr, nullptr)) { ... }

	// this is the only way to use it properly!
	if (0 != avformat_open_input(pop::out_ptr(raw_context, av_format_context_deleter{}), argv[0], nullptr, nullptr)) {
		std::stringstream ss;
		ss << "ffmpeg_image_loader could not open file '"
		   << path << "'";
		throw FFmpegInputException(ss.str().c_str());
	}

	return 0;
}
