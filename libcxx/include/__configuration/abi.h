// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___CONFIGURATION_ABI_H
#define _LIBCPP___CONFIGURATION_ABI_H

#include <__config_site>
#include <__configuration/compiler.h>
#include <__configuration/platform.h>

#ifndef _LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER
#  pragma GCC system_header
#endif

//
// ABI presets
//
#if _LIBCPP_ABI_VERSION >= 2
#  define _LIBCPP_ABI_ALTERNATE_STRING_LAYOUT 1
#  define _LIBCPP_ABI_INCOMPLETE_TYPES_IN_DEQUE 1
#  define _LIBCPP_ABI_LIST_REMOVE_NODE_POINTER_UB 1
#  define _LIBCPP_ABI_TREE_REMOVE_NODE_POINTER_UB 1
#  define _LIBCPP_ABI_FIX_UNORDERED_NODE_POINTER_UB 1
#  define _LIBCPP_ABI_FORWARD_LIST_REMOVE_NODE_POINTER_UB 1
#  define _LIBCPP_ABI_FIX_UNORDERED_CONTAINER_SIZE_TYPE 1
#  define _LIBCPP_ABI_BAD_FUNCTION_CALL_GOOD_WHAT_MESSAGE 1
#  define _LIBCPP_ABI_OPTIMIZED_LOCALE_NUM_GET 1
#  define _LIBCPP_ABI_NO_ITERATOR_BASES 1
#  define _LIBCPP_ABI_VARIANT_INDEX_TYPE_OPTIMIZATION 1
#  define _LIBCPP_ABI_OPTIMIZED_FUNCTION 1
#  define _LIBCPP_ABI_REGEX_CONSTANTS_NONZERO 1
#  define _LIBCPP_ABI_STRING_OPTIMIZED_EXTERNAL_INSTANTIATION 1
#  define _LIBCPP_ABI_ENABLE_UNIQUE_PTR_TRIVIAL_ABI 1
#  define _LIBCPP_ABI_ENABLE_SHARED_PTR_TRIVIAL_ABI 1
#  define _LIBCPP_ABI_NO_RANDOM_DEVICE_COMPATIBILITY_LAYOUT 1
#  define _LIBCPP_ABI_DO_NOT_EXPORT_BASIC_STRING_COMMON 1
#  define _LIBCPP_ABI_DO_NOT_EXPORT_VECTOR_BASE_COMMON 1
#  define _LIBCPP_ABI_BITSET_VECTOR_BOOL_CONST_SUBSCRIPT_RETURN_BOOL 1
#  define _LIBCPP_ABI_FIX_CITYHASH_IMPLEMENTATION 1
#  define _LIBCPP_ABI_DO_NOT_EXPORT_TO_CHARS_BASE_10 1
#  define _LIBCPP_ABI_USE_WRAP_ITER_IN_STD_ARRAY 1
#  define _LIBCPP_ABI_USE_WRAP_ITER_IN_STD_STRING_VIEW 1
#  define _LIBCPP_ABI_NO_FILESYSTEM_INLINE_NAMESPACE 1
#elif _LIBCPP_ABI_VERSION == 1
#  if !(defined(_LIBCPP_OBJECT_FORMAT_COFF) || defined(_LIBCPP_OBJECT_FORMAT_XCOFF))
#    define _LIBCPP_DEPRECATED_ABI_LEGACY_LIBRARY_DEFINITIONS_FOR_INLINE_FUNCTIONS 1
#  endif
#  if defined(__FreeBSD__) && __FreeBSD__ < 14
#    define _LIBCPP_DEPRECATED_ABI_DISABLE_PAIR_TRIVIAL_COPY_CTOR 1
#  endif
#endif

#if defined(_LIBCPP_COMPILER_CLANG_BASED)
#  if defined(__APPLE__)
#    if defined(__i386__) || defined(__x86_64__)
// use old string layout on x86_64 and i386
#    elif defined(__arm__)
// use old string layout on arm (which does not include aarch64/arm64), except on watch ABIs
#      if defined(__ARM_ARCH_7K__) && __ARM_ARCH_7K__ >= 2
#        define _LIBCPP_ABI_ALTERNATE_STRING_LAYOUT 1
#      endif
#    else
#      define _LIBCPP_ABI_ALTERNATE_STRING_LAYOUT 1
#    endif
#  endif
#endif

//
// ABI configuration macros
// ========================
//
// Each macro defined here can be overridden by setting it to 1/0 inside __config_site. These macros define the
// ABI configuration of the library, which is a stable property once set. This means that each combination of
// ABI macros gives rise to an ABI that is stable across all supported "usage scenarios" of the library
// (-fno-exceptions, compilers, standard versions, etc).
//
// If a usage scenario makes it impossible to honor a specific ABI configuration (such as the implementation
// requiring a specific compiler or standard mode), it should be a hard error at compile-time to try enabling
// that ABI configuration under that usage scenario. This ensures that if an ABI configuration is opted-into,
// that ABI cannot be silently broken under some usage scenarios.
//
// Each ABI configuration macro also defines a "signature element" which is used to encode the fact that this
// ABI macro is active and distinguish it from other configurations of the library. Amongst other things, this
// is then encoded in the inline namespace used by the library.
//
// Defining new ABI configurations
// -------------------------------
// First, think about whether the ABI configuration can be implemented under all supported usage scenarios. If
// that is not the case, carefully consider if that ABI configuration is still generally useful, and if so, make
// sure to trigger an error at compile-time when the ABI configuration cannot be honored.
//
// Then, define a macro with a descriptive name and default it to 0 (aka disabled). Also select an ABI signature
// element that will be encoded in the inline namespace when that ABI configuration is active. We recommend using
// single characters in sequence to keep symbol names short.
//
// Finally, make sure to add the new ABI configuration macro to the definition of the ABI signature. Look for the
// definition of the _LIBCPP_ABI_SIGNATURE macro.
//

// Change short string representation so that string data starts at offset 0,
// improving its alignment in some cases.
#ifndef _LIBCPP_ABI_ALTERNATE_STRING_LAYOUT
#  define _LIBCPP_ABI_ALTERNATE_STRING_LAYOUT 0
#endif
#if _LIBCPP_ABI_ALTERNATE_STRING_LAYOUT
#  define _LIBCPP_ABI_ALTERNATE_STRING_LAYOUT_SIG A
#else
#  define _LIBCPP_ABI_ALTERNATE_STRING_LAYOUT_SIG
#endif

// Fix deque iterator type in order to support incomplete types.
#ifndef _LIBCPP_ABI_INCOMPLETE_TYPES_IN_DEQUE
#  define _LIBCPP_ABI_INCOMPLETE_TYPES_IN_DEQUE 0
#endif
#if _LIBCPP_ABI_INCOMPLETE_TYPES_IN_DEQUE
#  define _LIBCPP_ABI_INCOMPLETE_TYPES_IN_DEQUE_SIG B
#else
#  define _LIBCPP_ABI_INCOMPLETE_TYPES_IN_DEQUE_SIG
#endif

// Fix undefined behavior in how std::list stores its linked nodes.
#ifndef _LIBCPP_ABI_LIST_REMOVE_NODE_POINTER_UB
#  define _LIBCPP_ABI_LIST_REMOVE_NODE_POINTER_UB 0
#endif
#if _LIBCPP_ABI_LIST_REMOVE_NODE_POINTER_UB
#  define _LIBCPP_ABI_LIST_REMOVE_NODE_POINTER_UB_SIG C
#else
#  define _LIBCPP_ABI_LIST_REMOVE_NODE_POINTER_UB_SIG
#endif

// Fix undefined behavior in how __tree stores its end and parent nodes.
#ifndef _LIBCPP_ABI_TREE_REMOVE_NODE_POINTER_UB
#  define _LIBCPP_ABI_TREE_REMOVE_NODE_POINTER_UB 0
#endif
#if _LIBCPP_ABI_TREE_REMOVE_NODE_POINTER_UB
#  define _LIBCPP_ABI_TREE_REMOVE_NODE_POINTER_UB_SIG D
#else
#  define _LIBCPP_ABI_TREE_REMOVE_NODE_POINTER_UB_SIG
#endif

// Fix undefined behavior in how __hash_table stores its pointer types.
#ifndef _LIBCPP_ABI_FIX_UNORDERED_NODE_POINTER_UB
#  define _LIBCPP_ABI_FIX_UNORDERED_NODE_POINTER_UB 0
#endif
#if _LIBCPP_ABI_FIX_UNORDERED_NODE_POINTER_UB
#  define _LIBCPP_ABI_FIX_UNORDERED_NODE_POINTER_UB_SIG E
#else
#  define _LIBCPP_ABI_FIX_UNORDERED_NODE_POINTER_UB_SIG
#endif

// Fix undefined behavior in how forward_list stores its pointer types.
#ifndef _LIBCPP_ABI_FORWARD_LIST_REMOVE_NODE_POINTER_UB
#  define _LIBCPP_ABI_FORWARD_LIST_REMOVE_NODE_POINTER_UB 0
#endif
#if _LIBCPP_ABI_FORWARD_LIST_REMOVE_NODE_POINTER_UB
#  define _LIBCPP_ABI_FORWARD_LIST_REMOVE_NODE_POINTER_UB_SIG F
#else
#  define _LIBCPP_ABI_FORWARD_LIST_REMOVE_NODE_POINTER_UB_SIG
#endif

#ifndef _LIBCPP_ABI_FIX_UNORDERED_CONTAINER_SIZE_TYPE
#  define _LIBCPP_ABI_FIX_UNORDERED_CONTAINER_SIZE_TYPE 0
#endif
#if _LIBCPP_ABI_FIX_UNORDERED_CONTAINER_SIZE_TYPE
#  define _LIBCPP_ABI_FIX_UNORDERED_CONTAINER_SIZE_TYPE_SIG G
#else
#  define _LIBCPP_ABI_FIX_UNORDERED_CONTAINER_SIZE_TYPE_SIG
#endif

// Override the default return value of exception::what() for bad_function_call::what()
// with a string that is specific to bad_function_call (see http://wg21.link/LWG2233).
// This is an ABI break on platforms that sign and authenticate vtable function pointers
// because it changes the mangling of the virtual function located in the vtable, which
// changes how it gets signed.
#ifndef _LIBCPP_ABI_BAD_FUNCTION_CALL_GOOD_WHAT_MESSAGE
#  define _LIBCPP_ABI_BAD_FUNCTION_CALL_GOOD_WHAT_MESSAGE 0
#endif
#if _LIBCPP_ABI_BAD_FUNCTION_CALL_GOOD_WHAT_MESSAGE
#  define _LIBCPP_ABI_BAD_FUNCTION_CALL_GOOD_WHAT_MESSAGE_SIG H
#else
#  define _LIBCPP_ABI_BAD_FUNCTION_CALL_GOOD_WHAT_MESSAGE_SIG
#endif

// Enable optimized version of __do_get_(un)signed which avoids redundant copies.
#ifndef _LIBCPP_ABI_OPTIMIZED_LOCALE_NUM_GET
#  define _LIBCPP_ABI_OPTIMIZED_LOCALE_NUM_GET 0
#endif
#if _LIBCPP_ABI_OPTIMIZED_LOCALE_NUM_GET
#  define _LIBCPP_ABI_OPTIMIZED_LOCALE_NUM_GET_SIG I
#else
#  define _LIBCPP_ABI_OPTIMIZED_LOCALE_NUM_GET_SIG
#endif

// Give reverse_iterator<T> one data member of type T, not two.
// Also, in C++17 and later, don't derive iterator types from std::iterator.
#ifndef _LIBCPP_ABI_NO_ITERATOR_BASES
#  define _LIBCPP_ABI_NO_ITERATOR_BASES 0
#endif
#if _LIBCPP_ABI_NO_ITERATOR_BASES
#  define _LIBCPP_ABI_NO_ITERATOR_BASES_SIG J
#else
#  define _LIBCPP_ABI_NO_ITERATOR_BASES_SIG
#endif

// Use the smallest possible integer type to represent the index of the variant.
// Previously libc++ used "unsigned int" exclusively.
#ifndef _LIBCPP_ABI_VARIANT_INDEX_TYPE_OPTIMIZATION
#  define _LIBCPP_ABI_VARIANT_INDEX_TYPE_OPTIMIZATION 0
#endif
#if _LIBCPP_ABI_VARIANT_INDEX_TYPE_OPTIMIZATION
#  define _LIBCPP_ABI_VARIANT_INDEX_TYPE_OPTIMIZATION_SIG K
#else
#  define _LIBCPP_ABI_VARIANT_INDEX_TYPE_OPTIMIZATION_SIG
#endif

// Unstable attempt to provide a more optimized std::function
#ifndef _LIBCPP_ABI_OPTIMIZED_FUNCTION
#  define _LIBCPP_ABI_OPTIMIZED_FUNCTION 0
#endif
#if _LIBCPP_ABI_OPTIMIZED_FUNCTION
#  define _LIBCPP_ABI_OPTIMIZED_FUNCTION_SIG L
#else
#  define _LIBCPP_ABI_OPTIMIZED_FUNCTION_SIG
#endif

// All the regex constants must be distinct and nonzero.
#ifndef _LIBCPP_ABI_REGEX_CONSTANTS_NONZERO
#  define _LIBCPP_ABI_REGEX_CONSTANTS_NONZERO 0
#endif
#if _LIBCPP_ABI_REGEX_CONSTANTS_NONZERO
#  define _LIBCPP_ABI_REGEX_CONSTANTS_NONZERO_SIG M
#else
#  define _LIBCPP_ABI_REGEX_CONSTANTS_NONZERO_SIG
#endif

// Re-worked external template instantiations for std::string with a focus on
// performance and fast-path inlining.
#ifndef _LIBCPP_ABI_STRING_OPTIMIZED_EXTERNAL_INSTANTIATION
#  define _LIBCPP_ABI_STRING_OPTIMIZED_EXTERNAL_INSTANTIATION 0
#endif
#if _LIBCPP_ABI_STRING_OPTIMIZED_EXTERNAL_INSTANTIATION
#  define _LIBCPP_ABI_STRING_OPTIMIZED_EXTERNAL_INSTANTIATION_SIG N
#else
#  define _LIBCPP_ABI_STRING_OPTIMIZED_EXTERNAL_INSTANTIATION_SIG
#endif

// Enable clang::trivial_abi on std::unique_ptr.
#ifndef _LIBCPP_ABI_ENABLE_UNIQUE_PTR_TRIVIAL_ABI
#  define _LIBCPP_ABI_ENABLE_UNIQUE_PTR_TRIVIAL_ABI 0
#endif
#if _LIBCPP_ABI_ENABLE_UNIQUE_PTR_TRIVIAL_ABI
#  define _LIBCPP_ABI_ENABLE_UNIQUE_PTR_TRIVIAL_ABI_SIG O
#else
#  define _LIBCPP_ABI_ENABLE_UNIQUE_PTR_TRIVIAL_ABI_SIG
#endif

// Enable clang::trivial_abi on std::shared_ptr and std::weak_ptr
#ifndef _LIBCPP_ABI_ENABLE_SHARED_PTR_TRIVIAL_ABI
#  define _LIBCPP_ABI_ENABLE_SHARED_PTR_TRIVIAL_ABI 0
#endif
#if _LIBCPP_ABI_ENABLE_SHARED_PTR_TRIVIAL_ABI
#  define _LIBCPP_ABI_ENABLE_SHARED_PTR_TRIVIAL_ABI_SIG P
#else
#  define _LIBCPP_ABI_ENABLE_SHARED_PTR_TRIVIAL_ABI_SIG
#endif

// std::random_device holds some state when it uses an implementation that gets
// entropy from a file (see _LIBCPP_USING_DEV_RANDOM). When switching from this
// implementation to another one on a platform that has already shipped
// std::random_device, one needs to retain the same object layout to remain ABI
// compatible. This switch removes these workarounds for platforms that don't care
// about ABI compatibility.
#ifndef _LIBCPP_ABI_NO_RANDOM_DEVICE_COMPATIBILITY_LAYOUT
#  define _LIBCPP_ABI_NO_RANDOM_DEVICE_COMPATIBILITY_LAYOUT 0
#endif
#if _LIBCPP_ABI_NO_RANDOM_DEVICE_COMPATIBILITY_LAYOUT
#  define _LIBCPP_ABI_NO_RANDOM_DEVICE_COMPATIBILITY_LAYOUT_SIG Q
#else
#  define _LIBCPP_ABI_NO_RANDOM_DEVICE_COMPATIBILITY_LAYOUT_SIG
#endif

// Don't export the legacy __basic_string_common class and its methods from the built library.
#ifndef _LIBCPP_ABI_DO_NOT_EXPORT_BASIC_STRING_COMMON
#  define _LIBCPP_ABI_DO_NOT_EXPORT_BASIC_STRING_COMMON 0
#endif
#if _LIBCPP_ABI_DO_NOT_EXPORT_BASIC_STRING_COMMON
#  define _LIBCPP_ABI_DO_NOT_EXPORT_BASIC_STRING_COMMON_SIG R
#else
#  define _LIBCPP_ABI_DO_NOT_EXPORT_BASIC_STRING_COMMON_SIG
#endif

// Don't export the legacy __vector_base_common class and its methods from the built library.
#ifndef _LIBCPP_ABI_DO_NOT_EXPORT_VECTOR_BASE_COMMON
#  define _LIBCPP_ABI_DO_NOT_EXPORT_VECTOR_BASE_COMMON 0
#endif
#if _LIBCPP_ABI_DO_NOT_EXPORT_VECTOR_BASE_COMMON
#  define _LIBCPP_ABI_DO_NOT_EXPORT_VECTOR_BASE_COMMON_SIG S
#else
#  define _LIBCPP_ABI_DO_NOT_EXPORT_VECTOR_BASE_COMMON_SIG
#endif

// According to the Standard, `bitset::operator[] const` returns bool
#ifndef _LIBCPP_ABI_BITSET_VECTOR_BOOL_CONST_SUBSCRIPT_RETURN_BOOL
#  define _LIBCPP_ABI_BITSET_VECTOR_BOOL_CONST_SUBSCRIPT_RETURN_BOOL 0
#endif
#if _LIBCPP_ABI_BITSET_VECTOR_BOOL_CONST_SUBSCRIPT_RETURN_BOOL
#  define _LIBCPP_ABI_BITSET_VECTOR_BOOL_CONST_SUBSCRIPT_RETURN_BOOL_SIG T
#else
#  define _LIBCPP_ABI_BITSET_VECTOR_BOOL_CONST_SUBSCRIPT_RETURN_BOOL_SIG
#endif

// Fix the implementation of CityHash used for std::hash<fundamental-type>.
// This is an ABI break because `std::hash` will return a different result,
// which means that hashing the same object in translation units built against
// different versions of libc++ can return inconsistent results. This is especially
// tricky since std::hash is used in the implementation of unordered containers.
//
// The incorrect implementation of CityHash has the problem that it drops some
// bits on the floor.
#ifndef _LIBCPP_ABI_FIX_CITYHASH_IMPLEMENTATION
#  define _LIBCPP_ABI_FIX_CITYHASH_IMPLEMENTATION 0
#endif
#if _LIBCPP_ABI_FIX_CITYHASH_IMPLEMENTATION
#  define _LIBCPP_ABI_FIX_CITYHASH_IMPLEMENTATION_SIG U
#else
#  define _LIBCPP_ABI_FIX_CITYHASH_IMPLEMENTATION_SIG
#endif

// Remove the base 10 implementation of std::to_chars from the dylib.
// The implementation moved to the header, but we still export the symbols from
// the dylib for backwards compatibility.
#ifndef _LIBCPP_ABI_DO_NOT_EXPORT_TO_CHARS_BASE_10
#  define _LIBCPP_ABI_DO_NOT_EXPORT_TO_CHARS_BASE_10 0
#endif
#if _LIBCPP_ABI_DO_NOT_EXPORT_TO_CHARS_BASE_10
#  define _LIBCPP_ABI_DO_NOT_EXPORT_TO_CHARS_BASE_10_SIG V
#else
#  define _LIBCPP_ABI_DO_NOT_EXPORT_TO_CHARS_BASE_10_SIG
#endif

// Define std::array/std::string_view iterators to be __wrap_iters instead of raw
// pointers, which prevents people from relying on a non-portable implementation
// detail. This is especially useful because enabling bounded iterators hardening
// requires code not to make these assumptions.
#ifndef _LIBCPP_ABI_USE_WRAP_ITER_IN_STD_ARRAY
#  define _LIBCPP_ABI_USE_WRAP_ITER_IN_STD_ARRAY 0
#endif
#if _LIBCPP_ABI_USE_WRAP_ITER_IN_STD_ARRAY
#  define _LIBCPP_ABI_USE_WRAP_ITER_IN_STD_ARRAY_SIG W
#else
#  define _LIBCPP_ABI_USE_WRAP_ITER_IN_STD_ARRAY_SIG
#endif

#ifndef _LIBCPP_ABI_USE_WRAP_ITER_IN_STD_STRING_VIEW
#  define _LIBCPP_ABI_USE_WRAP_ITER_IN_STD_STRING_VIEW 0
#endif
#if _LIBCPP_ABI_USE_WRAP_ITER_IN_STD_STRING_VIEW
#  define _LIBCPP_ABI_USE_WRAP_ITER_IN_STD_STRING_VIEW_SIG X
#else
#  define _LIBCPP_ABI_USE_WRAP_ITER_IN_STD_STRING_VIEW_SIG
#endif

// Changes the iterator type of select containers (see below) to a bounded iterator that keeps track of whether it's
// within the bounds of the original container and asserts it on every dereference.
//
// ABI impact: changes the iterator type of the relevant containers.
//
// Supported containers:
// - `span`;
// - `string_view`;
// - `array`.
#ifndef _LIBCPP_ABI_BOUNDED_ITERATORS
#  define _LIBCPP_ABI_BOUNDED_ITERATORS 0
#endif
#if _LIBCPP_ABI_BOUNDED_ITERATORS
#  define _LIBCPP_ABI_BOUNDED_ITERATORS_SIG x
#else
#  define _LIBCPP_ABI_BOUNDED_ITERATORS_SIG
#endif

// Dont' add an inline namespace for `std::filesystem`
#ifndef _LIBCPP_ABI_NO_FILESYSTEM_INLINE_NAMESPACE
#  define _LIBCPP_ABI_NO_FILESYSTEM_INLINE_NAMESPACE 0
#endif
#if _LIBCPP_ABI_NO_FILESYSTEM_INLINE_NAMESPACE
#  define _LIBCPP_ABI_NO_FILESYSTEM_INLINE_NAMESPACE_SIG Y
#else
#  define _LIBCPP_ABI_NO_FILESYSTEM_INLINE_NAMESPACE_SIG
#endif

// Enable compiling copies of now inline methods into the dylib to support
// applications compiled against older libraries. This is unnecessary with
// COFF dllexport semantics, since dllexport forces a non-inline definition
// of inline functions to be emitted anyway. Our own non-inline copy would
// conflict with the dllexport-emitted copy, so we disable it. For XCOFF,
// the linker will take issue with the symbols in the shared object if the
// weak inline methods get visibility (such as from -fvisibility-inlines-hidden),
// so disable it.
#ifndef _LIBCPP_DEPRECATED_ABI_LEGACY_LIBRARY_DEFINITIONS_FOR_INLINE_FUNCTIONS
#  define _LIBCPP_DEPRECATED_ABI_LEGACY_LIBRARY_DEFINITIONS_FOR_INLINE_FUNCTIONS 0
#endif
#if _LIBCPP_DEPRECATED_ABI_LEGACY_LIBRARY_DEFINITIONS_FOR_INLINE_FUNCTIONS
#  define _LIBCPP_DEPRECATED_ABI_LEGACY_LIBRARY_DEFINITIONS_FOR_INLINE_FUNCTIONS_SIG Z
#else
#  define _LIBCPP_DEPRECATED_ABI_LEGACY_LIBRARY_DEFINITIONS_FOR_INLINE_FUNCTIONS_SIG
#endif

#ifndef _LIBCPP_DEPRECATED_ABI_DISABLE_PAIR_TRIVIAL_COPY_CTOR
#  define _LIBCPP_DEPRECATED_ABI_DISABLE_PAIR_TRIVIAL_COPY_CTOR 0
#endif
#if _LIBCPP_DEPRECATED_ABI_DISABLE_PAIR_TRIVIAL_COPY_CTOR
#  define _LIBCPP_DEPRECATED_ABI_DISABLE_PAIR_TRIVIAL_COPY_CTOR_SIG z
#else
#  define _LIBCPP_DEPRECATED_ABI_DISABLE_PAIR_TRIVIAL_COPY_CTOR_SIG
#endif

#define _LIBCPP_ABI_CONCAT_IMPL(x, y) x##y
#define _LIBCPP_ABI_CONCAT(x, y) _LIBCPP_ABI_CONCAT_IMPL(x, y)

//
// Definition of the ABI signature of the library.
//
// This is a token that uniquely represents the current ABI configuration of the library and can be used
// to generate things like an identifier for an inline namespace.
//
// clang-format off
#define _LIBCPP_ABI_SIGNATURE                                                                                         \
  _LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(  \
  _LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(  \
  _LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(  \
  _LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(  \
  _LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(_LIBCPP_ABI_CONCAT(                                                           \
      _LIBCPP_ABI_ALTERNATE_STRING_LAYOUT_SIG,                                                                        \
      _LIBCPP_ABI_INCOMPLETE_TYPES_IN_DEQUE_SIG),                                                                     \
      _LIBCPP_ABI_LIST_REMOVE_NODE_POINTER_UB_SIG),                                                                   \
      _LIBCPP_ABI_TREE_REMOVE_NODE_POINTER_UB_SIG),                                                                   \
      _LIBCPP_ABI_FIX_UNORDERED_NODE_POINTER_UB_SIG),                                                                 \
      _LIBCPP_ABI_FORWARD_LIST_REMOVE_NODE_POINTER_UB),                                                               \
      _LIBCPP_ABI_FIX_UNORDERED_CONTAINER_SIZE_TYPE_SIG),                                                             \
      _LIBCPP_ABI_BAD_FUNCTION_CALL_GOOD_WHAT_MESSAGE_SIG),                                                           \
      _LIBCPP_ABI_OPTIMIZED_LOCALE_NUM_GET_SIG),                                                                      \
      _LIBCPP_ABI_NO_ITERATOR_BASES_SIG),                                                                             \
      _LIBCPP_ABI_VARIANT_INDEX_TYPE_OPTIMIZATION_SIG),                                                               \
      _LIBCPP_ABI_OPTIMIZED_FUNCTION_SIG),                                                                            \
      _LIBCPP_ABI_REGEX_CONSTANTS_NONZERO_SIG),                                                                       \
      _LIBCPP_ABI_STRING_OPTIMIZED_EXTERNAL_INSTANTIATION_SIG),                                                       \
      _LIBCPP_ABI_ENABLE_UNIQUE_PTR_TRIVIAL_ABI_SIG),                                                                 \
      _LIBCPP_ABI_ENABLE_SHARED_PTR_TRIVIAL_ABI_SIG),                                                                 \
      _LIBCPP_ABI_NO_RANDOM_DEVICE_COMPATIBILITY_LAYOUT_SIG),                                                         \
      _LIBCPP_ABI_DO_NOT_EXPORT_BASIC_STRING_COMMON_SIG),                                                             \
      _LIBCPP_ABI_DO_NOT_EXPORT_VECTOR_BASE_COMMON_SIG),                                                              \
      _LIBCPP_ABI_BITSET_VECTOR_BOOL_CONST_SUBSCRIPT_RETURN_BOOL_SIG),                                                \
      _LIBCPP_ABI_FIX_CITYHASH_IMPLEMENTATION_SIG),                                                                   \
      _LIBCPP_ABI_DO_NOT_EXPORT_TO_CHARS_BASE_10_SIG),                                                                \
      _LIBCPP_ABI_USE_WRAP_ITER_IN_STD_ARRAY_SIG),                                                                    \
      _LIBCPP_ABI_USE_WRAP_ITER_IN_STD_STRING_VIEW_SIG),                                                              \
      _LIBCPP_ABI_BOUNDED_ITERATORS_SIG),                                                                             \
      _LIBCPP_ABI_NO_FILESYSTEM_INLINE_NAMESPACE_SIG),                                                                \
      _LIBCPP_DEPRECATED_ABI_LEGACY_LIBRARY_DEFINITIONS_FOR_INLINE_FUNCTIONS_SIG),                                    \
      _LIBCPP_DEPRECATED_ABI_DISABLE_PAIR_TRIVIAL_COPY_CTOR_SIG)                                                      \
/**/
// clang-format on

// We had some bugs where we use [[no_unique_address]] together with construct_at,
// which causes UB as the call on construct_at could write to overlapping subobjects
//
// https://github.com/llvm/llvm-project/issues/70506
// https://github.com/llvm/llvm-project/issues/70494
//
// To fix the bug we had to change the ABI of some classes to remove [[no_unique_address]] under certain conditions.
// The macro below is used for all classes whose ABI have changed as part of fixing these bugs.
#define _LIBCPP_ABI_LLVM18_NO_UNIQUE_ADDRESS __attribute__((__abi_tag__("llvm18_nua")))

#endif // _LIBCPP___CONFIGURATION_ABI_H
