//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___CONFIGURATION_EXPERIMENTAL_H
#define _LIBCPP___CONFIGURATION_EXPERIMENTAL_H

#include <__config_site>

#ifndef _LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER
#  pragma GCC system_header
#endif

#if __has_feature(experimental_library)
#  ifndef _LIBCPP_ENABLE_EXPERIMENTAL
#    define _LIBCPP_ENABLE_EXPERIMENTAL
#  endif
#endif

// Incomplete features get their own specific disabling flags. This makes it
// easier to grep for target specific flags once the feature is complete.
#if defined(_LIBCPP_ENABLE_EXPERIMENTAL) || defined(_LIBCPP_BUILDING_LIBRARY)
#  define _LIBCPP_HAS_EXPERIMENTAL_LIBRARY 1
#else
#  define _LIBCPP_HAS_EXPERIMENTAL_LIBRARY 0
#endif

#define _LIBCPP_HAS_EXPERIMENTAL_TZDB _LIBCPP_HAS_EXPERIMENTAL_LIBRARY
#define _LIBCPP_HAS_EXPERIMENTAL_SYNCSTREAM _LIBCPP_HAS_EXPERIMENTAL_LIBRARY
#define _LIBCPP_HAS_EXPERIMENTAL_HARDENING_OBSERVE_SEMANTIC _LIBCPP_HAS_EXPERIMENTAL_LIBRARY
#define _LIBCPP_HAS_EXPERIMENTAL_OPTIONAL_ITERATOR _LIBCPP_HAS_EXPERIMENTAL_LIBRARY

// The parallel algorithms themselves are not experimental, but the parallel backends that
// actually run work concurrently still are. When this is 0, the PSTL runs everything serially
// regardless of which backend the vendor selected via LIBCXX_PSTL_BACKEND. See
// <__pstl/backend_fwd.h> for how this selects the backend configuration.
//
// Note that this is one of the rare experimental features that changes the body of a function
// which also exists when experimental features are disabled. That's why
// <__configuration/attributes.h> folds _LIBCPP_EXPERIMENTAL_SIG into _LIBCPP_ODR_SIGNATURE.
#define _LIBCPP_HAS_EXPERIMENTAL_PSTL_PARALLEL_BACKEND _LIBCPP_HAS_EXPERIMENTAL_LIBRARY

#endif // _LIBCPP___CONFIGURATION_EXPERIMENTAL_H
