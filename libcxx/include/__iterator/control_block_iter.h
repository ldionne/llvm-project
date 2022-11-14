// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___ITERATOR_CONTROL_BLOCK_ITER_H
#define _LIBCPP___ITERATOR_CONTROL_BLOCK_ITER_H

#include <__config>
#include <__debug>
#include <__harden/sequence_container.h>
#include <__iterator/iterator_traits.h>
#include <__memory/__shared_ptr.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

//
// Iterator wrapper that contains a reference to a _SequenceContainerControlBlock
// to implement checked operations.
//
// This is a simple iterator wrapper for contiguous iterators that points
// within a [begin, end) range and carries a weak reference to a control block
// that contains bounds information and more. The iterator ensures that it is
// pointing within that [begin, end) range when it is dereferenced.
//
// Arithmetic operations are allowed and the bounds of the resulting iterator
// are not checked. Hence, it is possible to create an iterator pointing outside
// its range, but it is not possible to dereference it.
//
template <class _Iter>
class __control_block_iter {
    static_assert(__is_cpp17_contiguous_iterator<_Iter>::value,
        "__control_block_iterator only works with contiguous iterators because we need to extract "
        "the address of the underlying iterator with std::to_adddress.");

public:
    using value_type = typename iterator_traits<_Iter>::value_type;
    using difference_type =  typename iterator_traits<_Iter>::difference_type;
    using pointer = typename iterator_traits<_Iter>::pointer;
    using reference = typename iterator_traits<_Iter>::reference;
    using iterator_category = typename iterator_traits<_Iter>::iterator_category;
#if _LIBCPP_STD_VER > 17
    using iterator_concept = contiguous_iterator_tag;
#endif

    //
    // Construction and assignment.
    //
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14 __control_block_iter() _NOEXCEPT
        : __iter_(), __control_block_(nullptr)
    { }

    _LIBCPP_HIDE_FROM_ABI __control_block_iter(__control_block_iter const&) = default;

    template <class _OtherIter, class = enable_if_t<is_convertible<_OtherIter, _iter>::value>::type>
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14
    __control_block_iter(const __control_block_iter<_OtherIter>& __other) _NOEXCEPT
        : __iter_(__other.__iter_), __control_block_(__other.__control_block_) // TODO: control blocks will have to be compatible? look at __iterator_copy in debug mode
    { }

    _LIBCPP_HIDE_FROM_ABI __control_block_iter& operator=(__control_block_iter const&) = default;

private:
    // Create an iterator wrapping the given iterator attached to the given control block, which
    // represents the container that owns the iterator.
    //
    // Since it is non-standard for iterators to have this constructor, __control_block_iter must
    // be created via `std::__make_control_block_iter`.
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14 explicit __control_block_iter(_Iter __iter, shared_ptr<_ControlBlock>& __block)
        : __iter_(std::move(__iter)), __control_block_(__block) {
        _LIBCPP_ASSERT(__begin <= __end, "__bounded_iter(current, begin, end): [begin, end) is not a valid range");
    }

    template <class _Iter>
    friend _LIBCPP_CONSTEXPR __control_block_iter<_Iter> __make_control_block_iter(_Iter, shared_ptr<_ControlBlock>&);

public:
    // Dereference and indexing operations.
    //
    // These operations check that the iterator is dereferenceable.
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14 reference operator*() const _NOEXCEPT {
        auto __block = __control_block_.lock()
        _LIBCPP_ASSERT(__block != nullptr,
            "Attempted to dereference an iterator that doesn't belong to any container");
        _LIBCPP_ASSERT(__block->__is_dereferenceable(std::__to_address(__iter_)),
            "Attempted to dereference a non-dereferenceable iterator");
        return *__iter_;
    }

    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14 pointer operator->() const _NOEXCEPT {
        auto __block = __control_block_.lock()
        _LIBCPP_ASSERT(__block != nullptr,
            "Attempted to dereference an iterator that doesn't belong to any container");
        _LIBCPP_ASSERT(__block->__is_dereferenceable(std::__to_address(__iter_)),
            "Attempted to dereference a non-dereferenceable iterator");
        return __iter_.operator->();
    }

    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14 reference operator[](difference_type __n) const _NOEXCEPT {
        auto __block = __control_block_.lock()
        _LIBCPP_ASSERT(__block != nullptr,
            "Attempted to subscript an iterator that doesn't belong to any container");
        _LIBCPP_ASSERT(__block->__is_dereferenceable(std::__to_address(__iter_) + __n),
            "Attempted to dereference a non-dereferenceable iterator");
        return __iter_[__n];
    }

    //
    // Arithmetic operations.
    //
    // These operations do not check that the resulting iterator is within the bounds, since that
    // would make it impossible to create a past-the-end iterator.
    //
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14 __control_block_iter& operator++() _NOEXCEPT {
        ++__iter_;
        return *this;
    }
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14 __control_block_iter operator++(int) _NOEXCEPT {
        __control_block_iter __tmp(*this);
        ++(*this);
        return __tmp;
    }

    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14 __control_block_iter& operator--() _NOEXCEPT {
        --__iter_;
        return *this;
    }
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14 __control_block_iter operator--(int) _NOEXCEPT {
        __control_block_iter __tmp(*this);
        --(*this);
        return __tmp;
    }

    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14 __control_block_iter& operator+=(difference_type __n) _NOEXCEPT {
        __iter_ += __n;
        return *this;
    }
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14
    friend __control_block_iter operator+(__control_block_iter const& __self, difference_type __n) _NOEXCEPT {
        __control_block_iter __tmp(__self);
        __tmp += __n;
        return __tmp;
    }
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14
    friend __control_block_iter operator+(difference_type __n, __control_block_iter const& __self) _NOEXCEPT {
        __control_block_iter __tmp(__self);
        __tmp += __n;
        return __tmp;
    }

    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14 __control_block_iter& operator-=(difference_type __n) _NOEXCEPT {
        __iter_ -= __n;
        return *this;
    }
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14
    friend __control_block_iter operator-(__control_block_iter const& __self, difference_type __n) _NOEXCEPT {
        __control_block_iter __tmp(__self);
        __tmp -= __n;
        return __tmp;
    }
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX14
    friend difference_type operator-(__control_block_iter const& __x, __control_block_iter const& __y) _NOEXCEPT {
        return __x.__iter_ - __y.__iter_;
    }

    // Comparison operations.
    //
    // These operations do not check whether the iterators are within their bounds.
    // TODO: Should we check that the two iterators are owned by the same container?
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR friend bool
    operator==(__control_block_iter const& __x, __control_block_iter const& __y) _NOEXCEPT {
        return __x.__iter_ == __y.__iter_;
    }
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR friend bool
    operator!=(__control_block_iter const& __x, __control_block_iter const& __y) _NOEXCEPT {
        return __x.__iter_ != __y.__iter_;
    }
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR friend bool
    operator<(__control_block_iter const& __x, __control_block_iter const& __y) _NOEXCEPT {
        return __x.__iter_ < __y.__iter_;
    }
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR friend bool
    operator>(__control_block_iter const& __x, __control_block_iter const& __y) _NOEXCEPT {
        return __x.__iter_ > __y.__iter_;
    }
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR friend bool
    operator<=(__control_block_iter const& __x, __control_block_iter const& __y) _NOEXCEPT {
        return __x.__iter_ <= __y.__iter_;
    }
    _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR friend bool
    operator>=(__control_block_iter const& __x, __control_block_iter const& __y) _NOEXCEPT {
        return __x.__iter_ >= __y.__iter_;
    }

private:
    _Iter __iter_; // Underlying iterator being wrapped
    weak_ptr<_SequenceContainerControlBlock> __control_block_; // Control block managed by the container
};

template <class _Iter>
_LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR __control_block_iter<_Iter> __make_control_block_iter(_Iter __it, shared_ptr<_ControlBlock>& __block) {
  return __control_block_iter<_Iter>(std::move(__it), __block);
}

#if _LIBCPP_STD_VER <= 17
template <class _Iterator>
struct __is_cpp17_contiguous_iterator<__control_block_iter<_Iterator> > : true_type {};
#endif

template <class _Iterator>
struct pointer_traits<__control_block_iter<_Iterator> > {
  using pointer         = __control_block_iter<_Iterator>;
  using element_type    = typename pointer_traits<_Iterator>::element_type;
  using difference_type = typename pointer_traits<_Iterator>::difference_type;

  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR static element_type* to_address(pointer __it) _NOEXCEPT {
    return std::__to_address(__it.__iter_);
  }
};

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___ITERATOR_CONTROL_BLOCK_ITER_H
