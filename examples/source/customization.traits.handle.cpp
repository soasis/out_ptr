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

// A template that always evaluates to false anyhow
template <std::size_t I>
struct always_false_index : std::integral_constant<bool, I == 1 && I == 0> {};

namespace ztd { namespace out_ptr {

	// this is the simple customization point
	// you don't get access to the underlying storage,
	// but you can utilize the ::pointer typedef to
	// cutsomize the state utilized
	template <typename T, typename D, typename Pointer>
	class inout_ptr_traits<ztd::handle<T, D>, Pointer> {
	private:
		using Smart		 = ztd::handle<T, D>;
		using source_pointer = pointer_of_or_t<ztd::handle<T, D>, Pointer>;

		struct optimized_pointer_state {
			Pointer* target;

			optimized_pointer_state(Pointer* target_ptr) noexcept
			: target(target_ptr) {
			}

			optimized_pointer_state(optimized_pointer_state&& right) noexcept
			: target(right.target) {
			}

			optimized_pointer_state& operator=(optimized_pointer_state&& right) noexcept {
				target = std::move(right.target);
			}

			explicit operator bool() const noexcept {
				return target != nullptr;
			}
		};

	public:
		using pointer = optimized_pointer_state;

		static pointer construct(Smart& s) {
			return pointer(reinterpret_cast<Pointer*>(std::addressof(s.get())));
		}

		static Pointer* get(Smart&, pointer& state) noexcept {
			return state.target;
		}

		static void reset(Smart&, pointer&) noexcept {
			// value already written directly into the pointer
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
