//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// <numeric>

// template <class InputIterator1, class InputIterator2, class T,
//           class BinaryOperation1, class BinaryOperation2>
//    T transform_reduce(InputIterator1 first1, InputIterator1 last1,
//                       InputIterator2 first2, T init,
//                       BinaryOperation1 binary_op1, BinaryOperation2 binary_op2);
//
// Mandates: All of
//  - binary_op1(init, init)
//  - binary_op1(init, binary_op2(*first1, *first2))
//  - binary_op1(binary_op2(*first1, *first2), init)
//  - binary_op1(binary_op2(*first1, *first2), binary_op2(*first1, *first2))
// are convertible to T.

#include <numeric>
#include <type_traits>

struct NotConvertible {};

struct Init {
  Init& operator=(NotConvertible const&) { return *this; }
};

struct TransformedValue {};

void f(int* first, int* last, Init init) {
  // binary_op(init, init) is not convertible to Init
  {
    auto op = []([[maybe_unused]] auto x, [[maybe_unused]] auto y) {
      if constexpr (std::is_same_v<decltype(x), Init> && std::is_same_v<decltype(y), Init>) {
        return NotConvertible{};
      } else {
        return Init{};
      }
    };
    auto transform = [](auto, auto) { return TransformedValue{}; };
    std::transform_reduce(
        first, last, first, init, op, transform); // expected-error@*:* {{static assertion failed due to requirement}}
  }

  // binary_op(init, binary_op2(*first1, *first2)) is not convertible to Init
  {
    auto op = []([[maybe_unused]] auto x, [[maybe_unused]] auto y) {
      if constexpr (std::is_same_v<decltype(x), Init> && std::is_same_v<decltype(y), TransformedValue>) {
        return NotConvertible{};
      } else {
        return Init{};
      }
    };
    auto transform = [](auto, auto) { return TransformedValue{}; };
    std::transform_reduce(
        first, last, first, init, op, transform); // expected-error@*:* {{static assertion failed due to requirement}}
  }

  // binary_op(binary_op2(*first1, *first2), init) is not convertible to Init
  {
    auto op = []([[maybe_unused]] auto x, [[maybe_unused]] auto y) {
      if constexpr (std::is_same_v<decltype(x), TransformedValue> && std::is_same_v<decltype(y), Init>) {
        return NotConvertible{};
      } else {
        return Init{};
      }
    };
    auto transform = [](auto, auto) { return TransformedValue{}; };
    std::transform_reduce(
        first, last, first, init, op, transform); // expected-error@*:* {{static assertion failed due to requirement}}
  }

  // binary_op(binary_op2(*first1, *first2), binary_op2(*first1, *first2)) is not convertible to Init
  {
    auto op = []([[maybe_unused]] auto x, [[maybe_unused]] auto y) {
      if constexpr (std::is_same_v<decltype(x), TransformedValue> && std::is_same_v<decltype(y), TransformedValue>) {
        return NotConvertible{};
      } else {
        return Init{};
      }
    };
    auto transform = [](auto, auto) { return TransformedValue{}; };
    std::transform_reduce(
        first, last, first, init, op, transform); // expected-error@*:* {{static assertion failed due to requirement}}
  }
}
