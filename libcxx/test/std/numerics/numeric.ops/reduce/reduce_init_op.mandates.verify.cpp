//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// <numeric>

// template<class InputIterator, class T, class BinaryOperation>
//   T reduce(InputIterator first, InputIterator last, T init, BinaryOperation op);
//
// Mandates: All of
//  - binary_op(init, *first)
//  - binary_op(*first, init)
//  - binary_op(init, init)
//  - binary_op(*first, *first)
// are convertible to T.

#include <numeric>
#include <type_traits>

struct NotConvertible {};

struct Init {
  Init& operator=(NotConvertible const&) { return *this; }
};

void f(int* first, int* last, Init init) {
  // binary_op(init, *first) is not convertible to Init
  {
    auto op = []([[maybe_unused]] auto x, [[maybe_unused]] auto y) {
      if constexpr (std::is_same_v<decltype(x), Init> && std::is_same_v<decltype(y), int>) {
        return NotConvertible{};
      } else {
        return Init{};
      }
    };
    std::reduce(first, last, init, op); // expected-error@*:* {{static assertion failed due to requirement}}
  }

  // binary_op(*first, init) is not convertible to Init
  {
    auto op = []([[maybe_unused]] auto x, [[maybe_unused]] auto y) {
      if constexpr (std::is_same_v<decltype(x), int> && std::is_same_v<decltype(y), Init>) {
        return NotConvertible{};
      } else {
        return Init{};
      }
    };
    std::reduce(first, last, init, op); // expected-error@*:* {{static assertion failed due to requirement}}
  }

  // binary_op(init, init) is not convertible to Init
  {
    auto op = []([[maybe_unused]] auto x, [[maybe_unused]] auto y) {
      if constexpr (std::is_same_v<decltype(x), Init> && std::is_same_v<decltype(y), Init>) {
        return NotConvertible{};
      } else {
        return Init{};
      }
    };
    std::reduce(first, last, init, op); // expected-error@*:* {{static assertion failed due to requirement}}
  }

  // binary_op(*first, *first) is not convertible to Init
  {
    auto op = []([[maybe_unused]] auto x, [[maybe_unused]] auto y) {
      if constexpr (std::is_same_v<decltype(x), int> && std::is_same_v<decltype(y), int>) {
        return NotConvertible{};
      } else {
        return Init{};
      }
    };
    std::reduce(first, last, init, op); // expected-error@*:* {{static assertion failed due to requirement}}
  }
}
