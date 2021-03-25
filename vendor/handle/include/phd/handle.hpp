//  Copyright ⓒ 2018-2021 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#pragma once

#ifndef ZTD_HANDLE_HANDLE_HPP
#define ZTD_HANDLE_HANDLE_HPP

#include <ztd/out_ptr/pointer_of.hpp>

#include <type_traits>
#include <utility>
#include <memory>

namespace ztd {

	template <typename T>
	struct default_handle_deleter {
		using pointer = T;

		static void write_null(pointer& p) noexcept {
			p = T();
		}

		static bool is_null(const pointer& p) noexcept {
			return p == T();
		}

		void operator()(const pointer&) const noexcept {
			// no-op: override the default deleter
			// for handle-specific behavior
		}
	};

	template <typename T>
	struct default_handle_deleter<T*> : std::default_delete<T> {
		using pointer = T*;

		static void write_null(pointer& p) noexcept {
			p = pointer();
		}

		static bool is_null(const pointer& p) noexcept {
			return p == pointer();
		}
	};

	namespace op_detail {

		struct has_write_null {
			template <typename T, typename P,
				typename U = typename std::remove_const<typename std::remove_reference<T>::type>::type,
				typename Y = decltype(std::declval<U&>().write_null(std::declval<P&>()))>
			static std::true_type test(int);

			template <typename...>
			static std::false_type test(...);
		};

		struct has_is_null {
			template <typename T, typename P,
				typename U = typename std::remove_const<typename std::remove_reference<T>::type>::type,
				typename Y = decltype(std::declval<U&>().is_null(std::declval<P&>()))>
			static std::true_type test(int);

			template <typename...>
			static std::false_type test(...);
		};

		template <typename D, typename P>
		void write_null(std::true_type, D& deleter, P&& p) noexcept {
			deleter.write_null(std::forward<P>(p));
		}

		template <typename D, typename P>
		void write_null(std::false_type, D&, P&& p) noexcept {
			using X = typename std::remove_const<typename std::remove_reference<P>::type>::type;
			default_handle_deleter<X>::write_null(std::forward<P>(p));
		}

		template <typename D, typename P>
		void write_null(D& deleter, P&& p) noexcept {
			using yes_no = decltype(has_write_null::test<D, typename std::remove_const<typename std::remove_reference<P>::type>::type>(0));
			write_null(yes_no(), deleter, std::forward<P>(p));
		}

		template <typename D, typename P>
		bool is_null(std::true_type, D& deleter, P&& p) noexcept {
			return deleter.is_null(std::forward<P>(p));
		}

		template <typename D, typename P>
		bool is_null(std::false_type, D&, P&& p) noexcept {
			return default_handle_deleter<typename std::remove_const<typename std::remove_reference<P>::type>::type>::is_null(std::forward<P>(p));
		}

		template <typename D, typename P>
		bool is_null(D& deleter, P&& p) noexcept {
			using yes_no = decltype(has_is_null::test<D, typename std::remove_const<typename std::remove_reference<P>::type>::type>(0));
			return is_null(yes_no(), deleter, std::forward<P>(p));
		}
	} // namespace op_detail

	template <typename T,
		typename Dx = default_handle_deleter<T>>
	struct handle : Dx {
	public:
		using pointer	 = ztd::out_ptr::pointer_type_t<T, Dx>;
		using deleter_type = Dx;

	private:
		using deleter_base = deleter_type;

		pointer res;

	public:
		handle() noexcept {
		}

		handle(pointer h) noexcept
		: res(h) {
		}

		handle(std::nullptr_t) noexcept {
			deleter_type& deleter = get_deleter();
			op_detail::write_null(deleter, res);
		}

		handle(pointer h, deleter_type d) noexcept
		: deleter_base(std::move(d)), res(h) {
		}

		handle(std::nullptr_t, deleter_type d) noexcept
		: deleter_base(std::move(d)) {
			deleter_type& deleter = get_deleter();
			op_detail::write_null(deleter, res);
		}

		template <typename... DxArgs>
		handle(pointer h, DxArgs&&... dx_args) noexcept
		: deleter_base(std::forward<DxArgs>(dx_args)...), res(h) {
		}

		template <typename... DxArgs>
		handle(std::nullptr_t, DxArgs&&... dx_args) noexcept
		: deleter_base(std::forward<Dx>(dx_args)...) {
			deleter_type& deleter = get_deleter();
			op_detail::write_null(deleter, res);
		}

		handle(const handle& nocopy) noexcept = delete;
		handle(handle&& mov) noexcept
		: deleter_base(std::move(mov)), res(std::move(mov.res)) {
			deleter_type& deleter = get_deleter();
			mov.reset(mov.get_null());
		}

		handle& operator=(const handle&) noexcept = delete;

		handle& operator=(handle&& right) noexcept {
			this->reset(right.release());
			return *this;
		}
		handle& operator=(pointer right) noexcept {
			this->reset(right);
			return *this;
		}
		handle& operator=(std::nullptr_t) noexcept {
			this->reset(nullptr);
			return *this;
		}

		explicit operator bool() const noexcept {
			return this->is_null();
		}

		pointer get_null() const noexcept {
			pointer p;
			const deleter_type& deleter = this->get_deleter();
			op_detail::write_null(deleter, p);
			return p;
		}

		static pointer get_null(const deleter_type& deleter) noexcept {
			pointer p;
			op_detail::write_null(deleter, p);
			return p;
		}

		bool is_null() const noexcept {
			const deleter_type& deleter = this->get_deleter();
			return op_detail::is_null(deleter, res);
		}

		static bool is_null(const deleter_type& deleter, pointer& res) noexcept {
			return op_detail::is_null(deleter, res);
		}

		pointer& get() noexcept {
			return res;
		}

		const pointer& get() const noexcept {
			return res;
		}

		void reset(pointer h) noexcept {
			deleter_type& deleter = this->get_deleter();
			if (!is_null())
				deleter(res);
			res = h;
		}

		void reset(std::nullptr_t = nullptr) noexcept {
			deleter_type& deleter = this->get_deleter();
			if (!is_null())
				deleter(res);
			op_detail::write_null(deleter, res);
		}

		pointer release() noexcept {
			pointer rel		  = std::move(res);
			deleter_type& deleter = this->get_deleter();
			op_detail::write_null(deleter, res);
			return rel;
		}

		void swap(handle& other) noexcept {
			std::swap(other.get_deleter(), this->get_deleter());
			std::swap(other.res, res);
		}

		deleter_type& get_deleter() noexcept {
			deleter_type& deleter = *this;
			return deleter;
		}

		const deleter_type& get_deleter() const noexcept {
			const deleter_type& deleter = *this;
			return deleter;
		}

		pointer operator*() noexcept {
			return get();
		}

		pointer operator->() noexcept {
			return get();
		}

		~handle() noexcept {
			deleter_type& deleter = this->get_deleter();
			if (!is_null())
				deleter(res);
			op_detail::write_null(deleter, res);
		}
	};

	template <typename T, typename Dx>
	inline bool operator==(const handle<T, Dx>& left, const handle<T, Dx>& right) noexcept {
		return left.get() == right.get();
	}
	template <typename T, typename Dx>
	inline bool operator==(std::nullptr_t, const handle<T, Dx>& left) noexcept {
		return left.is_null();
	}
	template <typename T, typename Dx>
	inline bool operator==(const handle<T, Dx>& left, std::nullptr_t) noexcept {
		return left.is_null();
	}
	template <typename T, typename Dx>
	inline bool operator==(typename handle<T, Dx>::pointer right, const handle<T, Dx>& left) noexcept {
		return left.get() == right;
	}
	template <typename T, typename Dx>
	inline bool operator==(const handle<T, Dx>& left, typename handle<T, Dx>::pointer right) noexcept {
		return left.get() == right;
	}

	template <typename T, typename Dx>
	inline bool operator!=(const handle<T, Dx>& left, const handle<T, Dx>& right) noexcept {
		return left.get() != right.get();
	}
	template <typename T, typename Dx>
	inline bool operator!=(std::nullptr_t right, const handle<T, Dx>& left) noexcept {
		return left.get() != left.get_null();
	}
	template <typename T, typename Dx>
	inline bool operator!=(const handle<T, Dx>& left, std::nullptr_t right) noexcept {
		return left.get() != left.get_null();
	}
	template <typename T, typename Dx>
	inline bool operator!=(typename handle<T, Dx>::pointer right, const handle<T, Dx>& left) noexcept {
		return left.get() != right;
	}
	template <typename T, typename Dx>
	inline bool operator!=(const handle<T, Dx>& left, typename handle<T, Dx>::pointer right) noexcept {
		return left.get() != right;
	}
} // namespace ztd

#endif // ZTD_HANDLE_HANDLE_HPP
