//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.


#include <phd/handle.hpp>

#include <ficapi/ficapi.hpp>

#include <phd/out_ptr.hpp>

#include <assert.hpp>

#include <type_traits>

// A template that always evaluates to false anyhow
template <std::size_t I>
struct always_false_index : std::integral_constant<bool, I == 1 && I == 0> {};

namespace phd { namespace out_ptr {

	// this is the simple customization point
	// you don't get access to the underlying storage,
	// but you can utilize the ::pointer typedef to
	// cutsomize the state utilized
	template <typename T, typename D, typename Pointer>
	class inout_ptr_traits<phd::handle<T, D>, Pointer> {
	private:
		using Smart		 = phd::handle<T, D>;
		using source_pointer = pointer_of_or_t<phd::handle<T, D>, Pointer>;

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

		static void reset(Smart& s, pointer& state) noexcept {
			// value already written directly into the pointer
		}
	};
}} // namespace phd::out_ptr

int main() {
	phd::handle<int*, ficapi::int_deleter> p(nullptr);
	ficapi_re_create(phd::out_ptr::inout_ptr<void*>(p), ficapi_type::ficapi_type_int);
	int* rawp = static_cast<int*>(p.get());

	OUT_PTR_C_ASSERT(rawp != nullptr);
	OUT_PTR_C_ASSERT(*rawp == ficapi_get_dynamic_data());
}
