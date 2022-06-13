// Copyright ⓒ 2018-2022 ThePhD.
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

#pragma once

#ifndef ZTD_OUT_PTR_FRIENDLY_INOUT_PTR_HPP
#define ZTD_OUT_PTR_FRIENDLY_INOUT_PTR_HPP

#if defined(ZTD_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR) && ZTD_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR != 0

#include <ztd/out_ptr/detail/base_inout_ptr_impl.hpp>
#include <ztd/out_ptr/pointer_of.hpp>

#include <cstdlib>
#include <cstddef>
#include <type_traits>
#include <memory>

namespace ztd { namespace out_ptr {

	namespace op_detail {
		template <typename Smart, typename Pointer, typename Args, typename List>
		struct friendly_inout_ptr_impl : public base_inout_ptr_impl<Smart, Pointer, Args, List> {
		private:
			using base_t = base_inout_ptr_impl<Smart, Pointer, Args, List>;

		public:
			using base_t::base_t;
		};

		template <typename T, typename D, typename Pointer>
		struct friendly_inout_ptr_impl<std::friendly_unique_ptr<T, D>, Pointer, std::tuple<>, ztd::out_ptr::op_detail::index_sequence<>> {
		public:
			using Smart		 = std::friendly_unique_ptr<T, D>;
			using source_pointer = pointer_of_or_t<Smart, Pointer>;

		private:
			Pointer* m_target_ptr;

		public:
			friendly_inout_ptr_impl(Smart& ptr, std::tuple<>) noexcept
			: m_target_ptr(static_cast<Pointer*>(static_cast<void*>(&ptr.friendly_get()))) {
			}

			friendly_inout_ptr_impl(friendly_inout_ptr_impl&& right) noexcept		  = default;
			friendly_inout_ptr_impl& operator=(friendly_inout_ptr_impl&& right) noexcept = default;
			friendly_inout_ptr_impl(const friendly_inout_ptr_impl&)				  = delete;
			friendly_inout_ptr_impl& operator=(const friendly_inout_ptr_impl&)		  = delete;

			operator Pointer*() const noexcept {
				return const_cast<Pointer*>(m_target_ptr);
			}
		};
	} // namespace op_detail

	template <typename Smart, typename Pointer, typename... Args>
	class friendly_inout_ptr_t : public op_detail::friendly_inout_ptr_impl<Smart, Pointer, std::tuple<Args...>, ztd::out_ptr::op_detail::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>> {
	private:
		using list_t = ztd::out_ptr::op_detail::make_index_sequence<std::tuple_size<std::tuple<Args...>>::value>;
		using core_t = op_detail::friendly_inout_ptr_impl<Smart, Pointer, std::tuple<Args...>, list_t>;

	public:
		friendly_inout_ptr_t(Smart& s, Args... args)
		: core_t(s, std::forward_as_tuple(std::forward<Args>(args)...)) {
		}
	};

	template <typename Pointer, typename Smart, typename... Args>
	friendly_inout_ptr_t<Smart, Pointer, Args...> friendly_inout_ptr(Smart& p, Args&&... args) noexcept {
		return friendly_inout_ptr_t<Smart, Pointer, Args...>(p, std::forward<Args>(args)...);
	}

	template <typename Smart, typename... Args>
	friendly_inout_ptr_t<Smart, pointer_of_t<Smart>, Args...> friendly_inout_ptr(Smart& p, Args&&... args) noexcept {
		using Pointer = pointer_of_t<Smart>;
		return friendly_inout_ptr<Pointer>(p, std::forward<Args>(args)...);
	}

}} // namespace ztd::out_ptr

#endif // OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR

#endif // ZTD_OUT_PTR_FRIENDLY_INOUT_PTR_HPP
