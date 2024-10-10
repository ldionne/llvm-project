//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___STRING_REPRESENTATION_H
#define _LIBCPP___STRING_REPRESENTATION_H

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

// This file defines possible representations for std::string.
//
// These representations are centralized around the idea that a string carries
// the following information (although it can be encoded in various ways):
//  - A pointer to the data in the string
//  - The size of the data currently stored in the string
//  - The storage capacity available in the string
//
// All representation classes must provide the following interface:
//
//  TODO
//
//

template <class _CharT, class _Pointer, class _SizeType>
union __classic_string_layout {
private:
  // Attribute 'packed' is used to keep the layout compatible with the
  // previous definition that did not use bit fields. This is because on
  // some platforms bit fields have a default size rather than the actual
  // size used, e.g., it is 4 bytes on AIX. See D128285 for details.
  struct __long {
    struct _LIBCPP_PACKED {
      _SizeType __is_long_ : 1;
      _SizeType __cap_ : sizeof(_SizeType) * CHAR_BIT - 1;
    };
    _SizeType __size_;
    _Pointer __data_;
  };

  static constexpr size_t
      __min_cap = (sizeof(__long) - 1) / sizeof(_CharT) > 2 ? (sizeof(__long) - 1) / sizeof(_CharT) : 2;

  struct __short {
    struct _LIBCPP_PACKED {
      unsigned char __is_long_ : 1;
      unsigned char __size_    : 7;
    };
    _LIBCPP_NO_UNIQUE_ADDRESS __padding<sizeof(_CharT) - 1> __padding_;
    _CharT __data_[__min_cap];
  };

  static_assert(sizeof(__short) == (sizeof(_CharT) * (__min_cap + 1)), "__short has an unexpected size.");

  // Actual members (remember, this is a union)
  __short __s;
  __long __l;

public:
#ifdef _LIBCPP_BIG_ENDIAN
  static const _SizeType __endian_factor = 1;
#else
  static const _SizeType __endian_factor = 2;
#endif

  //
  // API specific to the short string representation
  //
  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 _LIBCPP_STRING_INTERNAL_MEMORY_ACCESS void
  __set_short_size(_SizeType __s) _NOEXCEPT {
    _LIBCPP_ASSERT_INTERNAL(__is_short(), "String has to be short when trying to set the short size");
    _LIBCPP_ASSERT_INTERNAL(__s < __min_cap, "__s should never be greater than or equal to the short string capacity");
    __s.__size_    = __s;
    __s.__is_long_ = false; // TODO: it's weird to do this here
  }

  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 _LIBCPP_STRING_INTERNAL_MEMORY_ACCESS _SizeType
  __get_short_size() const _NOEXCEPT {
    _LIBCPP_ASSERT_INTERNAL(__is_short(), "String has to be short when trying to get the short size");
    return __s.__size_;
  }

  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 _LIBCPP_STRING_INTERNAL_MEMORY_ACCESS pointer
  __get_short_pointer() _NOEXCEPT {
    _LIBCPP_ASSERT_INTERNAL(__is_short(), "String has to be short when trying to get the short pointer");
    return _LIBCPP_ASAN_VOLATILE_WRAPPER(pointer_traits<pointer>::pointer_to(__s.__data_[0]));
  }

  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 _LIBCPP_STRING_INTERNAL_MEMORY_ACCESS const_pointer
  __get_short_pointer() const _NOEXCEPT {
    _LIBCPP_ASSERT_INTERNAL(__is_short(), "String has to be short when trying to get the short pointer");
    return _LIBCPP_ASAN_VOLATILE_WRAPPER(pointer_traits<const_pointer>::pointer_to(__s.__data_[0]));
  }

  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 _SizeType __get_short_cap() const _NOEXCEPT { return __min_cap; }

  //
  // API specific to the long string representation
  //
  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 void __set_long_size(_SizeType __s) _NOEXCEPT {
    _LIBCPP_ASSERT_INTERNAL(__is_long(), "String has to be long when trying to set the long size");
    __l.__size_ = __s;
  }

  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 _SizeType __get_long_size() const _NOEXCEPT {
    _LIBCPP_ASSERT_INTERNAL(__is_long(), "String has to be long when trying to get the long size");
    return __l.__size_;
  }

  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 void __set_long_pointer(pointer __p) _NOEXCEPT {
    // TODO: It looks like we don't have the guarantee that this is already a long string, which is strange
    __l.__data_ = __p;
  }

  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 pointer __get_long_pointer() _NOEXCEPT {
    _LIBCPP_ASSERT_INTERNAL(__is_long(), "String has to be long when trying to get the long pointer");
    return _LIBCPP_ASAN_VOLATILE_WRAPPER(__l.__data_);
  }
  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 const_pointer __get_long_pointer() const _NOEXCEPT {
    _LIBCPP_ASSERT_INTERNAL(__is_long(), "String has to be long when trying to get the long pointer");
    return _LIBCPP_ASAN_VOLATILE_WRAPPER(__l.__data_);
  }

  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 _SizeType __get_long_cap() const _NOEXCEPT {
    _LIBCPP_ASSERT_INTERNAL(__is_long(), "String has to be long when trying to get the long capacity");
    return __l.__cap_ * __endian_factor;
  }
  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 void __set_long_cap(_SizeType __s) _NOEXCEPT {
    // TODO: It looks like we don't have the guarantee that this is already a long string, which is strange.
    __l.__cap_     = __s / __endian_factor;
    __l.__is_long_ = true; // TODO: It's weird to do this here
  }

  //
  // Short/long agnostic APIs
  //
  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 void __set_size(_SizeType __s) _NOEXCEPT {
    if (__is_long())
      __set_long_size(__s);
    else
      __set_short_size(__s);
  }

  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 pointer __get_pointer() _NOEXCEPT {
    return __is_long() ? __get_long_pointer() : __get_short_pointer();
  }
  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 const_pointer __get_pointer() const _NOEXCEPT {
    return __is_long() ? __get_long_pointer() : __get_short_pointer();
  }

  _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 _SizeType __get_capacity() const _NOEXCEPT {
    return (__is_long() ? __get_long_cap() : __get_short_cap()) - 1; // TODO: why -1?
  }
};

template <class _CharT, class _Pointer, class _SizeType>
class __alternate_string_layout {
  struct __long {
    _Pointer __data_;
    _SizeType __size_;
    _SizeType __cap_ : sizeof(_SizeType) * CHAR_BIT - 1;
    _SizeType __is_long_ : 1;
  };

  static constexpr size_t
      __min_cap = (sizeof(__long) - 1) / sizeof(_CharT) > 2 ? (sizeof(__long) - 1) / sizeof(_CharT) : 2;

  struct __short {
    _CharT __data_[__min_cap];
    _LIBCPP_NO_UNIQUE_ADDRESS __padding<sizeof(_CharT) - 1> __padding_;
    unsigned char __size_    : 7;
    unsigned char __is_long_ : 1;
  };

  union __rep {
    __short __s;
    __long __l;
  };

  // The __endian_factor is required because the field we use to store the size
  // has one fewer bit than it would if it were not a bitfield.
  //
  // If the LSB is used to store the short-flag in the short string representation,
  // we have to multiply the size by two when it is stored and divide it by two when
  // it is loaded to make sure that we always store an even number. In the long string
  // representation, we can ignore this because we can assume that we always allocate
  // an even amount of _CharTs.
  //
  // If the MSB is used for the short-flag, the max_size() is numeric_limits<_SizeType>::max() / 2.
  // This does not impact the short string representation, since we never need the MSB
  // for representing the size of a short string anyway.

#ifdef _LIBCPP_BIG_ENDIAN
  static const _SizeType __endian_factor = 2;
#else
  static const _SizeType __endian_factor = 1;
#endif
};

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif // _LIBCPP___STRING_REPRESENTATION_H
