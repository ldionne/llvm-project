//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// <numeric>

// template <class InputIterator1, class T,
//           class BinaryOperation, class UnaryOperation>
//    T transform_reduce(InputIterator1 first1, InputIterator1 last1,
//                       T init, BinaryOperation binary_op, UnaryOperation unary_op);
//
// Mandates: All of
//  - binary_op(init, init)
//  - binary_op(init, unary_op(*first))
//  - binary_op(unary_op(*first), init)
//  - binary_op(unary_op(*first), unary_op(*first))
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
    auto transform = [](auto) { return TransformedValue{}; };
    std::transform_reduce(
        first, last, init, op, transform); // expected-error@*:* {{static assertion failed due to requirement}}
  }

  // binary_op(init, unary_op(*first)) is not convertible to Init
  {
    auto op = []([[maybe_unused]] auto x, [[maybe_unused]] auto y) {
      if constexpr (std::is_same_v<decltype(x), Init> && std::is_same_v<decltype(y), TransformedValue>) {
        return NotConvertible{};
      } else {
        return Init{};
      }
    };
    auto transform = [](auto) { return TransformedValue{}; };
    std::transform_reduce(
        first, last, init, op, transform); // expected-error@*:* {{static assertion failed due to requirement}}
  }

  // binary_op(unary_op(*first), init) is not convertible to Init
  {
    auto op = []([[maybe_unused]] auto x, [[maybe_unused]] auto y) {
      if constexpr (std::is_same_v<decltype(x), TransformedValue> && std::is_same_v<decltype(y), Init>) {
        return NotConvertible{};
      } else {
        return Init{};
      }
    };
    auto transform = [](auto) { return TransformedValue{}; };
    std::transform_reduce(
        first, last, init, op, transform); // expected-error@*:* {{static assertion failed due to requirement}}
  }

  // binary_op(unary_op(*first), unary_op(*first)) is not convertible to Init
  {
    auto op = []([[maybe_unused]] auto x, [[maybe_unused]] auto y) {
      if constexpr (std::is_same_v<decltype(x), TransformedValue> && std::is_same_v<decltype(y), TransformedValue>) {
        return NotConvertible{};
      } else {
        return Init{};
      }
    };
    auto transform = [](auto) { return TransformedValue{}; };
    std::transform_reduce(
        first, last, init, op, transform); // expected-error@*:* {{static assertion failed due to requirement}}
  }
}
