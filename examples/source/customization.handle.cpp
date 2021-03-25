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


#include <phd/handle.hpp>

#include <ficapi/ficapi.hpp>

#include <ztd/out_ptr.hpp>

#include <assert.hpp>

#include <type_traits>

namespace ztd { namespace out_ptr {

	// this is the full out_ptr_t customization point
	// you can also specialize the inout_ptr_t for that template too
	template <typename T, typename D, typename Pointer, typename... Args>
	class inout_ptr_t<ztd::handle<T, D>, Pointer, Args...> : std::tuple<Args...> {
	private:
		using Smart		 = ztd::handle<T, D>;
		using source_pointer = pointer_of_or_t<Smart, Pointer>;
		using ArgsTuple	 = std::tuple<Args...>;
		using Base		 = ArgsTuple;

		Pointer* m_target_ptr;

	public:
		inout_ptr_t(Smart& s, Args... args) noexcept
		: Base(std::forward<Args>(args)...), m_target_ptr(static_cast<Pointer*>(static_cast<void*>(std::addressof(s.get())))) {
		}

		inout_ptr_t(inout_ptr_t&& right) noexcept
		: Base(std::move(right)), m_target_ptr(right.m_target_ptr) {
			right.m_target_ptr = nullptr;
		}
		inout_ptr_t& operator=(inout_ptr_t&& right) noexcept {
			Base::operator	=(std::move(right));
			this->m_target_ptr = right.m_target_ptr;
			return *this;
		}

		operator Pointer*() const noexcept {
			return const_cast<Pointer*>(this->m_target_ptr);
		}

		~inout_ptr_t() noexcept {
			static_assert(sizeof...(Args) < 1, "you cannot reset the deleter or pass more arguments for handle<T, Deleter>!: it only takes one argument!");
			reset();
		}

	private:
		void reset() {
			// internal pointer has already been written into,
			// we are fine
		}
	};
}} // namespace ztd::out_ptr

int main() {
	ztd::handle<int*, ficapi::int_deleter> p(nullptr);
	ficapi_re_create(ztd::out_ptr::inout_ptr<void*>(p), ficapi_type::ficapi_type_int);
	int* rawp = static_cast<int*>(p.get());

	OUT_PTR_C_ASSERT(rawp != nullptr);
	OUT_PTR_C_ASSERT(*rawp == ficapi_get_dynamic_data());
}
