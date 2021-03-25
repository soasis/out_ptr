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

#pragma once

#ifndef ZTD_OUT_PTR_FRIENDLY_UNIQUE_PTR_HPP
#define ZTD_OUT_PTR_FRIENDLY_UNIQUE_PTR_HPP

#include <memory>

namespace std {

#if defined(_YVALS_CORE_H_)

#ifndef _NOEXCEPT
#define _NOEXCEPT noexcept
#endif // MSVC's noexcept is properly boxed from the user in later releases

#ifndef ZTD_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR
#define ZTD_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR 1
#endif

	template <class _Ty,
		class _Dx>
	class friendly_unique_ptr_base { // stores pointer and deleter
	public:
		typedef remove_reference_t<_Dx> _Dx_noref;
		typedef typename _Get_deleter_pointer_type<_Ty, _Dx_noref>::type pointer;

		template <class _Ptr2,
			class _Dx2>
		friendly_unique_ptr_base(_Ptr2 _Ptr, _Dx2&& _Dt)
		: _Mypair(_One_then_variadic_args_t(), ::std::forward<_Dx2>(_Dt), _Ptr) { // construct with compatible pointer and deleter
		}

		template <class _Ptr2>
		constexpr friendly_unique_ptr_base(_Ptr2 _Ptr)
		: _Mypair(_Zero_then_variadic_args_t(), _Ptr) { // construct with compatible pointer
		}

		_Dx& get_deleter() noexcept { // return reference to deleter
			return (_Mypair._Get_first());
		}

		 const _Dx& get_deleter() const noexcept { // return const reference to deleter
			return (_Mypair._Get_first());
		}

		pointer& _Myptr() noexcept { // return reference to pointer
#if _MSC_VER < 1920
			return (_Mypair._Get_second());
#else
			return (_Mypair._Myval2);
#endif
		}

		const pointer& _Myptr() const noexcept { // return const reference to pointer
#if _MSC_VER < 1920
			return (_Mypair._Get_second());
#else
			return (_Mypair._Myval2);
#endif
		}

		_Compressed_pair<_Dx, pointer> _Mypair;
	};

	template <class _Ty,
		class _Dx> // = default_delete<_Ty>
	class friendly_unique_ptr
	: public friendly_unique_ptr_base<_Ty, _Dx> { // non-copyable pointer to an object
	public:
		typedef friendly_unique_ptr_base<_Ty, _Dx> _Mybase;
		typedef typename _Mybase::pointer pointer;
		typedef _Ty element_type;
		typedef _Dx deleter_type;

		using _Mybase::get_deleter;

		template <class _Dx2				= _Dx,
			_Unique_ptr_enable_default_t<_Dx2> = 0>
		constexpr friendly_unique_ptr() noexcept
		: _Mybase(pointer()) { // default construct
		}

		template <class _Dx2				= _Dx,
			_Unique_ptr_enable_default_t<_Dx2> = 0>
		constexpr friendly_unique_ptr(nullptr_t) noexcept
		: _Mybase(pointer()) { // null pointer construct
		}

		friendly_unique_ptr& operator=(nullptr_t) noexcept { // assign a null pointer
			reset();
			return (*this);
		}

		template <class _Dx2				= _Dx,
			_Unique_ptr_enable_default_t<_Dx2> = 0>
		explicit friendly_unique_ptr(pointer _Ptr) noexcept
		: _Mybase(_Ptr) { // construct with pointer
		}

		template <class _Dx2								 = _Dx,
			enable_if_t<is_constructible_v<_Dx2, const _Dx2&>, int> = 0>
		friendly_unique_ptr(pointer _Ptr, const _Dx& _Dt) noexcept
		: _Mybase(_Ptr, _Dt) { // construct with pointer and (maybe const) deleter&
		}

		template <class _Dx2 = _Dx,
			enable_if_t<conjunction_v<negation<is_reference<_Dx2>>,
						  is_constructible<_Dx2, _Dx2>>,
				int>		 = 0>
		friendly_unique_ptr(pointer _Ptr, _Dx&& _Dt) noexcept
		: _Mybase(_Ptr, _STD move(_Dt)) { // construct by moving deleter
		}

		template <class _Dx2 = _Dx,
			enable_if_t<conjunction_v<is_reference<_Dx2>,
						  is_constructible<_Dx2, remove_reference_t<_Dx2>>>,
				int>		 = 0>
		friendly_unique_ptr(pointer, remove_reference_t<_Dx>&&) = delete;

		friendly_unique_ptr(friendly_unique_ptr&& _Right) noexcept
		: _Mybase(_Right.release(),
			  _STD forward<_Dx>(_Right.get_deleter())) { // construct by moving _Right
		}

		template <class _Ty2,
			class _Dx2,
			enable_if_t<conjunction_v<negation<is_array<_Ty2>>,
						  is_convertible<typename friendly_unique_ptr<_Ty2, _Dx2>::pointer, pointer>,
						  conditional_t<is_reference_v<_Dx>, is_same<_Dx2, _Dx>, is_convertible<_Dx2, _Dx>>>,
				int> = 0>
		friendly_unique_ptr(friendly_unique_ptr<_Ty2, _Dx2>&& _Right) noexcept
		: _Mybase(_Right.release(),
			  _STD forward<_Dx2>(_Right.get_deleter())) { // construct by moving _Right
		}

#if _HAS_AUTO_PTR_ETC
		template <class _Ty2,
			enable_if_t<conjunction_v<is_convertible<_Ty2*, _Ty*>,
						  is_same<_Dx, default_delete<_Ty>>>,
				int> = 0>
		friendly_unique_ptr(auto_ptr<_Ty2>&& _Right) noexcept
		: _Mybase(_Right.release()) { // construct by moving _Right
		}
#endif /* _HAS_AUTO_PTR_ETC */

		template <class _Ty2,
			class _Dx2,
			enable_if_t<conjunction_v<negation<is_array<_Ty2>>,
						  is_assignable<_Dx&, _Dx2>,
						  is_convertible<typename friendly_unique_ptr<_Ty2, _Dx2>::pointer, pointer>>,
				int> = 0>
		friendly_unique_ptr& operator=(friendly_unique_ptr<_Ty2, _Dx2>&& _Right) noexcept { // assign by moving _Right
			reset(_Right.release());
			this->get_deleter()	= _STD forward<_Dx2>(_Right.get_deleter());
			return (*this);
		}

		friendly_unique_ptr& operator=(friendly_unique_ptr&& _Right) noexcept { // assign by moving _Right
			if (this != _STD addressof(_Right)) {						  // different, do the move
				reset(_Right.release());
				this->get_deleter()= _STD forward<_Dx>(_Right.get_deleter());
			}
			return (*this);
		}

		void swap(friendly_unique_ptr& _Right) noexcept { // swap elements
			_Swap_adl(this->_Myptr(), _Right._Myptr());
			_Swap_adl(this->get_deleter(), _Right.get_deleter());
		}

		~friendly_unique_ptr() noexcept { // destroy the object
			if (get() != pointer()) {
				this->get_deleter()(get());
			}
		}

		add_lvalue_reference_t<_Ty> operator*() const { // return reference to object
			return (*get());
		}

		pointer operator->() const noexcept { // return pointer to class object
			return (this->_Myptr());
		}

		pointer get() const noexcept { // return pointer to object
			return (this->_Myptr());
		}

		pointer& friendly_get() noexcept { // return pointer to object
			return (this->_Myptr());
		}

		const pointer& friendly_get() const noexcept { // return pointer to object
			return (this->_Myptr());
		}

		explicit operator bool() const noexcept { // test for non-null pointer
			return (get() != pointer());
		}

		pointer release() noexcept { // yield ownership of pointer
			pointer _Ans   = get();
			this->_Myptr() = pointer();
			return (_Ans);
		}

		void reset(pointer _Ptr = pointer()) noexcept { // establish new pointer
			pointer _Old   = get();
			this->_Myptr() = _Ptr;
			if (_Old != pointer()) {
				this->get_deleter()(_Old);
			}
		}

		friendly_unique_ptr(const friendly_unique_ptr&) = delete;
		friendly_unique_ptr& operator=(const friendly_unique_ptr&) = delete;
	};

#elif defined(_LIBCPP_VERSION)

#ifndef ZTD_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR
#define ZTD_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR 1
#endif

	template <class _Tp, class _Dp = default_delete<_Tp>>
	class _LIBCPP_TEMPLATE_VIS friendly_unique_ptr {
	public:
		typedef _Tp element_type;
		typedef _Dp deleter_type;
		typedef typename __pointer_type<_Tp, deleter_type>::type pointer;

		static_assert(!is_rvalue_reference<deleter_type>::value,
			"the specified deleter type cannot be an rvalue reference");

	private:
		__compressed_pair<pointer, deleter_type> __ptr_;

		struct __nat {
			int __for_bool_;
		};

#ifndef _LIBCPP_CXX03_LANG
		typedef __unique_ptr_deleter_sfinae<_Dp> _DeleterSFINAE;

		template <bool _Dummy>
		using _LValRefType =
			typename __dependent_type<_DeleterSFINAE, _Dummy>::__lval_ref_type;

		template <bool _Dummy>
		using _GoodRValRefType =
			typename __dependent_type<_DeleterSFINAE, _Dummy>::__good_rval_ref_type;

		template <bool _Dummy>
		using _BadRValRefType =
			typename __dependent_type<_DeleterSFINAE, _Dummy>::__bad_rval_ref_type;

		template <bool _Dummy, class _Deleter = typename __dependent_type<__identity<deleter_type>, _Dummy>::type>
		using _EnableIfDeleterDefaultConstructible =
			typename enable_if<is_default_constructible<_Deleter>::value && !is_pointer<_Deleter>::value>::type;

		template <class _ArgType>
		using _EnableIfDeleterConstructible =
			typename enable_if<is_constructible<deleter_type, _ArgType>::value>::type;

		template <class _UPtr, class _Up>
		using _EnableIfMoveConvertible = typename enable_if<
			is_convertible<typename _UPtr::pointer, pointer>::value && !is_array<_Up>::value>::type;

		template <class _UDel>
		using _EnableIfDeleterConvertible = typename enable_if<
			(is_reference<_Dp>::value && is_same<_Dp, _UDel>::value) || (!is_reference<_Dp>::value && is_convertible<_UDel, _Dp>::value)>::type;

		template <class _UDel>
		using _EnableIfDeleterAssignable = typename enable_if<
			is_assignable<_Dp&, _UDel&&>::value>::type;

	public:
		template <bool _Dummy = true,
			class		  = _EnableIfDeleterDefaultConstructible<_Dummy>>
		_LIBCPP_INLINE_VISIBILITY constexpr friendly_unique_ptr() noexcept
		: __ptr_(pointer()) {
		}

		template <bool _Dummy = true,
			class		  = _EnableIfDeleterDefaultConstructible<_Dummy>>
		_LIBCPP_INLINE_VISIBILITY constexpr friendly_unique_ptr(nullptr_t) noexcept
		: __ptr_(pointer()) {
		}

		template <bool _Dummy = true,
			class		  = _EnableIfDeleterDefaultConstructible<_Dummy>>
		_LIBCPP_INLINE_VISIBILITY explicit friendly_unique_ptr(pointer __p) noexcept
		: __ptr_(__p) {
		}

		template <bool _Dummy = true,
			class		  = _EnableIfDeleterConstructible<_LValRefType<_Dummy>>>
		_LIBCPP_INLINE_VISIBILITY
		friendly_unique_ptr(pointer __p, _LValRefType<_Dummy> __d) noexcept
		: __ptr_(__p, __d) {
		}

		template <bool _Dummy = true,
			class		  = _EnableIfDeleterConstructible<_GoodRValRefType<_Dummy>>>
		_LIBCPP_INLINE_VISIBILITY
		friendly_unique_ptr(pointer __p, _GoodRValRefType<_Dummy> __d) noexcept
		: __ptr_(__p, _VSTD::move(__d)) {
			static_assert(!is_reference<deleter_type>::value,
				"rvalue deleter bound to reference");
		}

		template <bool _Dummy = true,
			class		  = _EnableIfDeleterConstructible<_BadRValRefType<_Dummy>>>
		_LIBCPP_INLINE_VISIBILITY
		friendly_unique_ptr(pointer __p, _BadRValRefType<_Dummy> __d)
			= delete;

		_LIBCPP_INLINE_VISIBILITY
		friendly_unique_ptr(friendly_unique_ptr&& __u) noexcept
		: __ptr_(__u.release(), _VSTD::forward<deleter_type>(__u.get_deleter())) {
		}

		template <class _Up, class _Ep,
			class = _EnableIfMoveConvertible<friendly_unique_ptr<_Up, _Ep>, _Up>,
			class = _EnableIfDeleterConvertible<_Ep>>
		_LIBCPP_INLINE_VISIBILITY
		friendly_unique_ptr(friendly_unique_ptr<_Up, _Ep>&& __u) _NOEXCEPT
		: __ptr_(__u.release(), _VSTD::forward<_Ep>(__u.get_deleter())) {
		}

#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
		template <class _Up>
		_LIBCPP_INLINE_VISIBILITY
		friendly_unique_ptr(auto_ptr<_Up>&& __p,
			typename enable_if<is_convertible<_Up*, _Tp*>::value && is_same<_Dp, default_delete<_Tp>>::value,
				__nat>::type
			= __nat()) _NOEXCEPT
		: __ptr_(__p.release()) {
		}
#endif

		_LIBCPP_INLINE_VISIBILITY
		friendly_unique_ptr& operator=(friendly_unique_ptr&& __u) _NOEXCEPT {
			reset(__u.release());
			__ptr_.second() = _VSTD::forward<deleter_type>(__u.get_deleter());
			return *this;
		}

		template <class _Up, class _Ep,
			class = _EnableIfMoveConvertible<friendly_unique_ptr<_Up, _Ep>, _Up>,
			class = _EnableIfDeleterAssignable<_Ep>>
		_LIBCPP_INLINE_VISIBILITY
			friendly_unique_ptr&
			operator=(friendly_unique_ptr<_Up, _Ep>&& __u) _NOEXCEPT {
			reset(__u.release());
			__ptr_.second() = _VSTD::forward<_Ep>(__u.get_deleter());
			return *this;
		}

#else  // _LIBCPP_CXX03_LANG
	private:
		friendly_unique_ptr(friendly_unique_ptr&);
		template <class _Up, class _Ep>
		friendly_unique_ptr(friendly_unique_ptr<_Up, _Ep>&);

		friendly_unique_ptr& operator=(friendly_unique_ptr&);
		template <class _Up, class _Ep>
		friendly_unique_ptr& operator=(friendly_unique_ptr<_Up, _Ep>&);

	public:
		_LIBCPP_INLINE_VISIBILITY
		friendly_unique_ptr()
		: __ptr_(pointer()) {
			static_assert(!is_pointer<deleter_type>::value,
				"friendly_unique_ptr constructed with null function pointer deleter");
			static_assert(is_default_constructible<deleter_type>::value,
				"friendly_unique_ptr::deleter_type is not default constructible");
		}
		_LIBCPP_INLINE_VISIBILITY
		friendly_unique_ptr(nullptr_t)
		: __ptr_(pointer()) {
			static_assert(!is_pointer<deleter_type>::value,
				"friendly_unique_ptr constructed with null function pointer deleter");
		}
		_LIBCPP_INLINE_VISIBILITY
		explicit friendly_unique_ptr(pointer __p)
		: __ptr_(_VSTD::move(__p)) {
			static_assert(!is_pointer<deleter_type>::value,
				"friendly_unique_ptr constructed with null function pointer deleter");
		}

		_LIBCPP_INLINE_VISIBILITY
		operator __rv<friendly_unique_ptr>() {
			return __rv<friendly_unique_ptr>(*this);
		}

		_LIBCPP_INLINE_VISIBILITY
		friendly_unique_ptr(__rv<friendly_unique_ptr> __u)
		: __ptr_(__u->release(),
			  _VSTD::forward<deleter_type>(__u->get_deleter())) {
		}

		template <class _Up, class _Ep>
		_LIBCPP_INLINE_VISIBILITY
			typename enable_if<
				!is_array<_Up>::value && is_convertible<typename friendly_unique_ptr<_Up, _Ep>::pointer, pointer>::value && is_assignable<deleter_type&, _Ep&>::value,
				friendly_unique_ptr&>::type
			operator=(friendly_unique_ptr<_Up, _Ep> __u) {
			reset(__u.release());
			__ptr_.second() = _VSTD::forward<_Ep>(__u.get_deleter());
			return *this;
		}

		_LIBCPP_INLINE_VISIBILITY
		friendly_unique_ptr(pointer __p, deleter_type __d)
		: __ptr_(_VSTD::move(__p), _VSTD::move(__d)) {
		}
#endif // _LIBCPP_CXX03_LANG

#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
		template <class _Up>
		_LIBCPP_INLINE_VISIBILITY
			typename enable_if<is_convertible<_Up*, _Tp*>::value && is_same<_Dp, default_delete<_Tp>>::value,
				friendly_unique_ptr&>::type
			operator=(auto_ptr<_Up> __p) {
			reset(__p.release());
			return *this;
		}
#endif

		_LIBCPP_INLINE_VISIBILITY
		~friendly_unique_ptr() {
			reset();
		}

		_LIBCPP_INLINE_VISIBILITY
		friendly_unique_ptr& operator=(nullptr_t) _NOEXCEPT {
			reset();
			return *this;
		}

		_LIBCPP_INLINE_VISIBILITY
		typename add_lvalue_reference<_Tp>::type
		operator*() const {
			return *__ptr_.first();
		}
		_LIBCPP_INLINE_VISIBILITY
		pointer operator->() const _NOEXCEPT {
			return __ptr_.first();
		}
		_LIBCPP_INLINE_VISIBILITY
		pointer get() const _NOEXCEPT {
			return __ptr_.first();
		}
		_LIBCPP_INLINE_VISIBILITY
		pointer& friendly_get() _NOEXCEPT {
			return __ptr_.first();
		}
		_LIBCPP_INLINE_VISIBILITY
		deleter_type& get_deleter() _NOEXCEPT {
			return __ptr_.second();
		}
		_LIBCPP_INLINE_VISIBILITY
		const deleter_type& get_deleter() const _NOEXCEPT {
			return __ptr_.second();
		}
		_LIBCPP_INLINE_VISIBILITY
		_LIBCPP_EXPLICIT operator bool() const _NOEXCEPT {
			return __ptr_.first() != nullptr;
		}

		_LIBCPP_INLINE_VISIBILITY
		pointer release() _NOEXCEPT {
			pointer __t    = __ptr_.first();
			__ptr_.first() = pointer();
			return __t;
		}

		_LIBCPP_INLINE_VISIBILITY
		void reset(pointer __p = pointer()) _NOEXCEPT {
			pointer __tmp  = __ptr_.first();
			__ptr_.first() = __p;
			if (__tmp)
				__ptr_.second()(__tmp);
		}

		_LIBCPP_INLINE_VISIBILITY
		void swap(friendly_unique_ptr& __u) _NOEXCEPT {
			__ptr_.swap(__u.__ptr_);
		}
	};

#elif defined(__GLIBCXX__)

#ifndef ZTD_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR
#define ZTD_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR 1
#endif

	/// 20.7.1.2 friendly_unique_ptr for single objects.
	template <typename _Tp, typename _Dp = default_delete<_Tp>>
	class friendly_unique_ptr {
		template <class _Up>
		using _DeleterConstraint =
			typename __uniq_ptr_impl<_Tp, _Up>::_DeleterConstraint::type;

		__uniq_ptr_impl<_Tp, _Dp> _M_t;

	public:
		using pointer	 = typename __uniq_ptr_impl<_Tp, _Dp>::pointer;
		using element_type = _Tp;
		using deleter_type = _Dp;

		// helper template for detecting a safe conversion from another
		// friendly_unique_ptr
		template <typename _Up, typename _Ep>
		using __safe_conversion_up = __and_<
			is_convertible<typename friendly_unique_ptr<_Up, _Ep>::pointer, pointer>,
			__not_<is_array<_Up>>,
			__or_<__and_<is_reference<deleter_type>,
					 is_same<deleter_type, _Ep>>,
				__and_<__not_<is_reference<deleter_type>>,
					is_convertible<_Ep, deleter_type>>>>;

		// Constructors.

		/// Default constructor, creates a friendly_unique_ptr that owns nothing.
		template <typename _Up = _Dp,
			typename		   = _DeleterConstraint<_Up>>
		constexpr friendly_unique_ptr() noexcept
		: _M_t() {
		}

		/** Takes ownership of a pointer.
       *
       * @param __p  A pointer to an object of @c element_type
       *
       * The deleter will be value-initialized.
       */
		template <typename _Up = _Dp,
			typename		   = _DeleterConstraint<_Up>>
		explicit friendly_unique_ptr(pointer __p) noexcept
		: _M_t(__p) {
		}

		/** Takes ownership of a pointer.
       *
       * @param __p  A pointer to an object of @c element_type
       * @param __d  A reference to a deleter.
       *
       * The deleter will be initialized with @p __d
       */
		friendly_unique_ptr(pointer __p,
			typename conditional<is_reference<deleter_type>::value,
				deleter_type, const deleter_type&>::type __d) noexcept
		: _M_t(__p, __d) {
		}

		/** Takes ownership of a pointer.
       *
       * @param __p  A pointer to an object of @c element_type
       * @param __d  An rvalue reference to a deleter.
       *
       * The deleter will be initialized with @p std::move(__d)
       */
		friendly_unique_ptr(pointer __p,
			typename remove_reference<deleter_type>::type&& __d) noexcept
		: _M_t(std::move(__p), std::move(__d)) {
			static_assert(!std::is_reference<deleter_type>::value,
				"rvalue deleter bound to reference");
		}

		/// Creates a friendly_unique_ptr that owns nothing.
		template <typename _Up = _Dp,
			typename		   = _DeleterConstraint<_Up>>
		constexpr friendly_unique_ptr(nullptr_t) noexcept
		: friendly_unique_ptr() {
		}

		// Move constructors.

		/// Move constructor.
		friendly_unique_ptr(friendly_unique_ptr&& __u) noexcept
		: _M_t(__u.release(), std::forward<deleter_type>(__u.get_deleter())) {
		}

		/** @brief Converting constructor from another type
       *
       * Requires that the pointer owned by @p __u is convertible to the
       * type of pointer owned by this object, @p __u does not own an array,
       * and @p __u has a compatible deleter type.
       */
		template <typename _Up, typename _Ep, typename = _Require<__safe_conversion_up<_Up, _Ep>, typename conditional<is_reference<_Dp>::value, is_same<_Ep, _Dp>, is_convertible<_Ep, _Dp>>::type>>
		friendly_unique_ptr(friendly_unique_ptr<_Up, _Ep>&& __u) noexcept
		: _M_t(__u.release(), std::forward<_Ep>(__u.get_deleter())) {
		}

#if _GLIBCXX_USE_DEPRECATED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		/// Converting constructor from @c auto_ptr
		template <typename _Up, typename = _Require<is_convertible<_Up*, _Tp*>, is_same<_Dp, default_delete<_Tp>>>>
		friendly_unique_ptr(auto_ptr<_Up>&& __u) noexcept;
#pragma GCC diagnostic pop
#endif

		/// Destructor, invokes the deleter if the stored pointer is not null.
		~friendly_unique_ptr() noexcept {
			auto& __ptr = _M_t._M_ptr();
			if (__ptr != nullptr)
				get_deleter()(__ptr);
			__ptr = pointer();
		}

		// Assignment.

		/** @brief Move assignment operator.
       *
       * @param __u  The object to transfer ownership from.
       *
       * Invokes the deleter first if this object owns a pointer.
       */
		friendly_unique_ptr&
		operator=(friendly_unique_ptr&& __u) noexcept {
			reset(__u.release());
			get_deleter() = std::forward<deleter_type>(__u.get_deleter());
			return *this;
		}

		/** @brief Assignment from another type.
       *
       * @param __u  The object to transfer ownership from, which owns a
       *             convertible pointer to a non-array object.
       *
       * Invokes the deleter first if this object owns a pointer.
       */
		template <typename _Up, typename _Ep>
		typename enable_if<__and_<
						    __safe_conversion_up<_Up, _Ep>,
						    is_assignable<deleter_type&, _Ep&&>>::value,
			friendly_unique_ptr&>::type
		operator=(friendly_unique_ptr<_Up, _Ep>&& __u) noexcept {
			reset(__u.release());
			get_deleter() = std::forward<_Ep>(__u.get_deleter());
			return *this;
		}

		/// Reset the %friendly_unique_ptr to empty, invoking the deleter if necessary.
		friendly_unique_ptr&
		operator=(nullptr_t) noexcept {
			reset();
			return *this;
		}

		// Observers.

		/// Dereference the stored pointer.
		typename add_lvalue_reference<element_type>::type
		operator*() const {
			__glibcxx_assert(get() != pointer());
			return *get();
		}

		/// Return the stored pointer.
		pointer
		operator->() const noexcept {
			_GLIBCXX_DEBUG_PEDASSERT(get() != pointer());
			return get();
		}

		/// Return the stored pointer.
		pointer
		get() const noexcept {
			return _M_t._M_ptr();
		}

		/// Return the stored pointer.
		pointer&
		friendly_get() noexcept {
			return _M_t._M_ptr();
		}

		/// Return a reference to the stored deleter.
		deleter_type&
		get_deleter() noexcept {
			return _M_t._M_deleter();
		}

		/// Return a reference to the stored deleter.
		const deleter_type&
		get_deleter() const noexcept {
			return _M_t._M_deleter();
		}

		/// Return @c true if the stored pointer is not null.
		explicit operator bool() const noexcept {
			return get() == pointer() ? false : true;
		}

		// Modifiers.

		/// Release ownership of any stored pointer.
		pointer
		release() noexcept {
			pointer __p   = get();
			_M_t._M_ptr() = pointer();
			return __p;
		}

		/** @brief Replace the stored pointer.
       *
       * @param __p  The new pointer to store.
       *
       * The deleter will be invoked if a pointer is already owned.
       */
		void
		reset(pointer __p = pointer()) noexcept {
			using std::swap;
			swap(_M_t._M_ptr(), __p);
			if (__p != pointer())
				get_deleter()(__p);
		}

		/// Exchange the pointer and deleter with another object.
		void
		swap(friendly_unique_ptr& __u) noexcept {
			using std::swap;
			swap(_M_t, __u._M_t);
		}

		// Disable copy from lvalue.
		friendly_unique_ptr(const friendly_unique_ptr&) = delete;
		friendly_unique_ptr& operator=(const friendly_unique_ptr&) = delete;
	};

	/// 20.7.1.3 friendly_unique_ptr for array objects with a runtime length
	// [unique.ptr.runtime]
	// _GLIBCXX_RESOLVE_LIB_DEFECTS
	// DR 740 - omit specialization for array objects with a compile time length
	template <typename _Tp, typename _Dp>
	class friendly_unique_ptr<_Tp[], _Dp> {
		template <typename _Up>
		using _DeleterConstraint =
			typename __uniq_ptr_impl<_Tp, _Up>::_DeleterConstraint::type;

		__uniq_ptr_impl<_Tp, _Dp> _M_t;

		template <typename _Up>
		using __remove_cv = typename remove_cv<_Up>::type;

		// like is_base_of<_Tp, _Up> but false if unqualified types are the same
		template <typename _Up>
		using __is_derived_Tp
			= __and_<is_base_of<_Tp, _Up>,
				__not_<is_same<__remove_cv<_Tp>, __remove_cv<_Up>>>>;

	public:
		using pointer	 = typename __uniq_ptr_impl<_Tp, _Dp>::pointer;
		using element_type = _Tp;
		using deleter_type = _Dp;

		// helper template for detecting a safe conversion from another
		// friendly_unique_ptr
		template <typename _Up, typename _Ep,
			typename _Up_up		 = friendly_unique_ptr<_Up, _Ep>,
			typename _Up_element_type = typename _Up_up::element_type>
		using __safe_conversion_up = __and_<
			is_array<_Up>,
			is_same<pointer, element_type*>,
			is_same<typename _Up_up::pointer, _Up_element_type*>,
			is_convertible<_Up_element_type (*)[], element_type (*)[]>,
			__or_<__and_<is_reference<deleter_type>, is_same<deleter_type, _Ep>>,
				__and_<__not_<is_reference<deleter_type>>,
					is_convertible<_Ep, deleter_type>>>>;

		// helper template for detecting a safe conversion from a raw pointer
		template <typename _Up>
		using __safe_conversion_raw = __and_<
			__or_<__or_<is_same<_Up, pointer>,
					 is_same<_Up, nullptr_t>>,
				__and_<is_pointer<_Up>,
					is_same<pointer, element_type*>,
					is_convertible<
						typename remove_pointer<_Up>::type (*)[],
						element_type (*)[]>>>>;

		// Constructors.

		/// Default constructor, creates a friendly_unique_ptr that owns nothing.
		template <typename _Up = _Dp,
			typename		   = _DeleterConstraint<_Up>>
		constexpr friendly_unique_ptr() noexcept
		: _M_t() {
		}

		/** Takes ownership of a pointer.
       *
       * @param __p  A pointer to an array of a type safely convertible
       * to an array of @c element_type
       *
       * The deleter will be value-initialized.
       */
		template <typename _Up,
			typename _Vp = _Dp,
			typename	= _DeleterConstraint<_Vp>,
			typename	= typename enable_if<
				    __safe_conversion_raw<_Up>::value, bool>::type>
		explicit friendly_unique_ptr(_Up __p) noexcept
		: _M_t(__p) {
		}

		/** Takes ownership of a pointer.
       *
       * @param __p  A pointer to an array of a type safely convertible
       * to an array of @c element_type
       * @param __d  A reference to a deleter.
       *
       * The deleter will be initialized with @p __d
       */
		template <typename _Up,
			typename = typename enable_if<
				__safe_conversion_raw<_Up>::value, bool>::type>
		friendly_unique_ptr(_Up __p,
			typename conditional<is_reference<deleter_type>::value,
				deleter_type, const deleter_type&>::type __d) noexcept
		: _M_t(__p, __d) {
		}

		/** Takes ownership of a pointer.
       *
       * @param __p  A pointer to an array of a type safely convertible
       * to an array of @c element_type
       * @param __d  A reference to a deleter.
       *
       * The deleter will be initialized with @p std::move(__d)
       */
		template <typename _Up,
			typename = typename enable_if<
				__safe_conversion_raw<_Up>::value, bool>::type>
		friendly_unique_ptr(_Up __p, typename remove_reference<deleter_type>::type&& __d) noexcept
		: _M_t(std::move(__p), std::move(__d)) {
			static_assert(!is_reference<deleter_type>::value,
				"rvalue deleter bound to reference");
		}

		/// Move constructor.
		friendly_unique_ptr(friendly_unique_ptr&& __u) noexcept
		: _M_t(__u.release(), std::forward<deleter_type>(__u.get_deleter())) {
		}

		/// Creates a friendly_unique_ptr that owns nothing.
		template <typename _Up = _Dp,
			typename		   = _DeleterConstraint<_Up>>
		constexpr friendly_unique_ptr(nullptr_t) noexcept
		: friendly_unique_ptr() {
		}

		template <typename _Up, typename _Ep,
			typename = _Require<__safe_conversion_up<_Up, _Ep>>>
		friendly_unique_ptr(friendly_unique_ptr<_Up, _Ep>&& __u) noexcept
		: _M_t(__u.release(), std::forward<_Ep>(__u.get_deleter())) {
		}

		/// Destructor, invokes the deleter if the stored pointer is not null.
		~friendly_unique_ptr() {
			auto& __ptr = _M_t._M_ptr();
			if (__ptr != nullptr)
				get_deleter()(__ptr);
			__ptr = pointer();
		}

		// Assignment.

		/** @brief Move assignment operator.
       *
       * @param __u  The object to transfer ownership from.
       *
       * Invokes the deleter first if this object owns a pointer.
       */
		friendly_unique_ptr&
		operator=(friendly_unique_ptr&& __u) noexcept {
			reset(__u.release());
			get_deleter() = std::forward<deleter_type>(__u.get_deleter());
			return *this;
		}

		/** @brief Assignment from another type.
       *
       * @param __u  The object to transfer ownership from, which owns a
       *             convertible pointer to an array object.
       *
       * Invokes the deleter first if this object owns a pointer.
       */
		template <typename _Up, typename _Ep>
		typename enable_if<__and_<__safe_conversion_up<_Up, _Ep>,
						    is_assignable<deleter_type&, _Ep&&>>::value,
			friendly_unique_ptr&>::type
		operator=(friendly_unique_ptr<_Up, _Ep>&& __u) noexcept {
			reset(__u.release());
			get_deleter() = std::forward<_Ep>(__u.get_deleter());
			return *this;
		}

		/// Reset the %friendly_unique_ptr to empty, invoking the deleter if necessary.
		friendly_unique_ptr&
		operator=(nullptr_t) noexcept {
			reset();
			return *this;
		}

		// Observers.

		/// Access an element of owned array.
		typename std::add_lvalue_reference<element_type>::type
		operator[](size_t __i) const {
			__glibcxx_assert(get() != pointer());
			return get()[__i];
		}

		/// Return the stored pointer.
		pointer
		get() const noexcept {
			return _M_t._M_ptr();
		}

		/// Return a reference to the stored deleter.
		deleter_type&
		get_deleter() noexcept {
			return _M_t._M_deleter();
		}

		/// Return a reference to the stored deleter.
		const deleter_type&
		get_deleter() const noexcept {
			return _M_t._M_deleter();
		}

		/// Return @c true if the stored pointer is not null.
		explicit operator bool() const noexcept {
			return get() == pointer() ? false : true;
		}

		// Modifiers.

		/// Release ownership of any stored pointer.
		pointer
		release() noexcept {
			pointer __p   = get();
			_M_t._M_ptr() = pointer();
			return __p;
		}

		/** @brief Replace the stored pointer.
       *
       * @param __p  The new pointer to store.
       *
       * The deleter will be invoked if a pointer is already owned.
       */
		template <typename _Up,
			typename = _Require<
				__or_<is_same<_Up, pointer>,
					__and_<is_same<pointer, element_type*>,
						is_pointer<_Up>,
						is_convertible<
							typename remove_pointer<_Up>::type (*)[],
							element_type (*)[]>>>>>
		void
		reset(_Up __p) noexcept {
			pointer __ptr = __p;
			using std::swap;
			swap(_M_t._M_ptr(), __ptr);
			if (__ptr != nullptr)
				get_deleter()(__ptr);
		}

		void reset(nullptr_t = nullptr) noexcept {
			reset(pointer());
		}

		/// Exchange the pointer and deleter with another object.
		void
		swap(friendly_unique_ptr& __u) noexcept {
			using std::swap;
			swap(_M_t, __u._M_t);
		}

		// Disable copy from lvalue.
		friendly_unique_ptr(const friendly_unique_ptr&) = delete;
		friendly_unique_ptr& operator=(const friendly_unique_ptr&) = delete;
	};

	template <typename _Tp, typename _Dp>
	inline
#if __cplusplus > 201402L || !defined(__STRICT_ANSI__) // c++1z or gnu++11
		// Constrained free swap overload, see p0185r1
		typename enable_if<__is_swappable<_Dp>::value>::type
#else
		void
#endif
		swap(friendly_unique_ptr<_Tp, _Dp>& __x,
			friendly_unique_ptr<_Tp, _Dp>& __y) noexcept {
		__x.swap(__y);
	}

#if __cplusplus > 201402L || !defined(__STRICT_ANSI__) // c++1z or gnu++11
	template <typename _Tp, typename _Dp>
	typename enable_if<!__is_swappable<_Dp>::value>::type
	swap(friendly_unique_ptr<_Tp, _Dp>&,
		friendly_unique_ptr<_Tp, _Dp>&)
		= delete;
#endif

	template <typename _Tp, typename _Dp,
		typename _Up, typename _Ep>
	inline bool
	operator==(const friendly_unique_ptr<_Tp, _Dp>& __x,
		const friendly_unique_ptr<_Up, _Ep>& __y) {
		return __x.get() == __y.get();
	}

	template <typename _Tp, typename _Dp>
	inline bool
	operator==(const friendly_unique_ptr<_Tp, _Dp>& __x, nullptr_t) noexcept {
		return !__x;
	}

	template <typename _Tp, typename _Dp>
	inline bool
	operator==(nullptr_t, const friendly_unique_ptr<_Tp, _Dp>& __x) noexcept {
		return !__x;
	}

	template <typename _Tp, typename _Dp,
		typename _Up, typename _Ep>
	inline bool
	operator!=(const friendly_unique_ptr<_Tp, _Dp>& __x,
		const friendly_unique_ptr<_Up, _Ep>& __y) {
		return __x.get() != __y.get();
	}

	template <typename _Tp, typename _Dp>
	inline bool
	operator!=(const friendly_unique_ptr<_Tp, _Dp>& __x, nullptr_t) noexcept {
		return (bool)__x;
	}

	template <typename _Tp, typename _Dp>
	inline bool
	operator!=(nullptr_t, const friendly_unique_ptr<_Tp, _Dp>& __x) noexcept {
		return (bool)__x;
	}

	template <typename _Tp, typename _Dp,
		typename _Up, typename _Ep>
	inline bool
	operator<(const friendly_unique_ptr<_Tp, _Dp>& __x,
		const friendly_unique_ptr<_Up, _Ep>& __y) {
		typedef typename std::common_type<typename friendly_unique_ptr<_Tp, _Dp>::pointer,
			typename friendly_unique_ptr<_Up, _Ep>::pointer>::type _CT;
		return std::less<_CT>()(__x.get(), __y.get());
	}

	template <typename _Tp, typename _Dp>
	inline bool
	operator<(const friendly_unique_ptr<_Tp, _Dp>& __x, nullptr_t) {
		return std::less<typename friendly_unique_ptr<_Tp, _Dp>::pointer>()(__x.get(),
			nullptr);
	}

	template <typename _Tp, typename _Dp>
	inline bool
	operator<(nullptr_t, const friendly_unique_ptr<_Tp, _Dp>& __x) {
		return std::less<typename friendly_unique_ptr<_Tp, _Dp>::pointer>()(nullptr,
			__x.get());
	}

	template <typename _Tp, typename _Dp,
		typename _Up, typename _Ep>
	inline bool
	operator<=(const friendly_unique_ptr<_Tp, _Dp>& __x,
		const friendly_unique_ptr<_Up, _Ep>& __y) {
		return !(__y < __x);
	}

	template <typename _Tp, typename _Dp>
	inline bool
	operator<=(const friendly_unique_ptr<_Tp, _Dp>& __x, nullptr_t) {
		return !(nullptr < __x);
	}

	template <typename _Tp, typename _Dp>
	inline bool
	operator<=(nullptr_t, const friendly_unique_ptr<_Tp, _Dp>& __x) {
		return !(__x < nullptr);
	}

	template <typename _Tp, typename _Dp,
		typename _Up, typename _Ep>
	inline bool
	operator>(const friendly_unique_ptr<_Tp, _Dp>& __x,
		const friendly_unique_ptr<_Up, _Ep>& __y) {
		return (__y < __x);
	}

	template <typename _Tp, typename _Dp>
	inline bool
	operator>(const friendly_unique_ptr<_Tp, _Dp>& __x, nullptr_t) {
		return std::less<typename friendly_unique_ptr<_Tp, _Dp>::pointer>()(nullptr,
			__x.get());
	}

	template <typename _Tp, typename _Dp>
	inline bool
	operator>(nullptr_t, const friendly_unique_ptr<_Tp, _Dp>& __x) {
		return std::less<typename friendly_unique_ptr<_Tp, _Dp>::pointer>()(__x.get(),
			nullptr);
	}

	template <typename _Tp, typename _Dp,
		typename _Up, typename _Ep>
	inline bool
	operator>=(const friendly_unique_ptr<_Tp, _Dp>& __x,
		const friendly_unique_ptr<_Up, _Ep>& __y) {
		return !(__x < __y);
	}

	template <typename _Tp, typename _Dp>
	inline bool
	operator>=(const friendly_unique_ptr<_Tp, _Dp>& __x, nullptr_t) {
		return !(__x < nullptr);
	}

	template <typename _Tp, typename _Dp>
	inline bool
	operator>=(nullptr_t, const friendly_unique_ptr<_Tp, _Dp>& __x) {
		return !(nullptr < __x);
	}

#else

#define ZTD_OUT_PTR_HAS_FRIENDLY_UNIQUE_PTR 0

#endif // VC++ versus libc++ versus libstdc++ ...

} // namespace std

#endif // ZTD_OUT_PTR_FRIENDLY_UNIQUE_PTR_HPP
