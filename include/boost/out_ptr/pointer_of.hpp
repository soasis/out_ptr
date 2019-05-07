//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef BOOST_OUT_PTR_POINTER_OF_HPP
#define BOOST_OUT_PTR_POINTER_OF_HPP

#include <type_traits>
#include <memory>

namespace boost {
	namespace out_ptr_detail {
		struct disambiguate_ {
		};

		template <typename T, typename U, typename = void>
		struct element_type {
			using type = U;
		};

		template <typename T, typename U>
		struct element_type<T, U, std::void_t<typename T::element_type>> {
			using type = typename T::element_type*;
		};

		template <typename T, typename U, typename = void>
		struct pointer_of_or : element_type<T, U> {
		};

		template <typename T, typename U>
		struct pointer_of_or<T, U, std::void_t<typename T::pointer>> {
			using type = typename T::pointer;
		};

		template <typename T>
		struct has_typedef_pointer {
			typedef std::true_type yes;
			typedef std::false_type no;

			template <typename C>
			static yes& test(typename C::pointer*);

			template <typename>
			static no& test(...);

			static constexpr const bool value = std::is_same_v<decltype(test<T>(0)), yes>;
		};

		template <bool b, typename T, typename Fallback>
		struct pointer_typedef_enable_if {
		};

		template <typename T, typename Fallback>
		struct pointer_typedef_enable_if<false, T, Fallback> {
			typedef Fallback type;
		};

		template <typename T, typename Fallback>
		struct pointer_typedef_enable_if<true, T, Fallback> {
			typedef typename T::pointer type;
		};

		template <typename T, typename... Args>
		struct is_resetable_test {
		private:
			template <typename S>
			static std::true_type f(decltype(std::declval<S>().reset(std::declval<Args>()...))*);
			template <typename S>
			static std::false_type f(...);

		public:
			constexpr static bool value = std::is_same_v<decltype(f<T>(0)), std::true_type>;
		};

		template <typename T, typename = void>
		struct is_releasable : std::false_type {
		};

		template <typename T>
		struct is_releasable<T, std::void_t<decltype(std::declval<T&>().release())>> : std::true_type {
		};

		template <typename T, typename... Args>
		struct is_resetable : std::integral_constant<bool, is_resetable_test<T, Args...>::value> {
		};
	} // namespace out_ptr_detail

	template <typename T, typename U>
	struct pointer_of_or : out_ptr_detail::pointer_of_or<T, U> {
	};

	template <typename T, typename U>
	using pointer_of_or_t = typename pointer_of_or<T, U>::type;

	template <typename T>
	using pointer_of = pointer_of_or<T, typename std::pointer_traits<T>::element_type*>;

	template <typename T>
	using pointer_of_t = typename pointer_of<T>::type;

	template <typename T, typename Dx>
	struct pointer_type {
		typedef typename out_ptr_detail::pointer_typedef_enable_if<out_ptr_detail::has_typedef_pointer<Dx>::value, Dx, T>::type type;
	};

	template <typename T, typename D>
	using pointer_type_t = typename pointer_type<T, D>::type;

} // namespace boost

#endif // BOOST_OUT_PTR_POINTER_OF_HPP
