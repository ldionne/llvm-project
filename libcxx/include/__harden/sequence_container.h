// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___HARDEN_SEQUENCE_CONTAINER_H
#define _LIBCPP___HARDEN_SEQUENCE_CONTAINER_H

#include <__config>
#include <__memory/pointer_traits.h>
#include <__memory/shared_ptr.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _SizeType, class _Pointer, >
struct _SequenceContainerControlBlock {
    bool __owns(_ConstIterator const& __it) const {
        auto __block = __get_control_block(__it).lock();
        // If the control block has expired, this is a dangling iterator.
        if (__block == nullptr)
            return false;
        return *this == __block;
    }

    bool __is_dereferenceable(_Pointer __ptr) const {
        return __ptr >= __begin_ && __ptr < __end_;
    }

    void __update_bounds(_Pointer __begin, _Pointer __end) {
        __begin_ = __begin;
        __end_ = __end;
    }

private:
    _Pointer __begin_;
    _Pointer __end_;
};

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___HARDEN_SEQUENCE_CONTAINER_H
