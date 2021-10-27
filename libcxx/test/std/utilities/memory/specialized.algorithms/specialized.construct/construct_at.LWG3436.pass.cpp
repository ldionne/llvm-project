//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// Only recent Clang versions support array placement new in constant expressions.
// XFAIL: clang-12, clang-13, apple-clang-12, apple-clang-13

// This test ensures that we implement the resolution of https://wg21.link/LWG3436,
// which mentions that std::construct_at doesn't work with array types.
//
// Libc++ currently implements this LWG issue even though it hasn't been voted
// into the Standard yet, because failure to implement this issue makes it
// impossible (well, very difficult) to implement other features, such as support
// for arrays in shared_ptr.
//
// Note: libc++ currently implements the proposed resolution as of 2021-01-16.
//       If the proposed resolution that ends up being adopted changes, we'll
//       want to update our implementation (and probably this test too).
//
// REQUIRES: stdlib=libc++

#include <memory>
#include <cassert>
#include <concepts>

struct CountInitializations {
  CountInitializations() { ++constructions; }
  static int constructions;
};

int CountInitializations::constructions = 0;

// Note that we don't test std::construct_at(&array, arguments...) because that
// is ill-formed. That would call array placement new with a new-initializer
// like '( expression-list )', which is not allowed (arrays require a braced-init-list).
constexpr bool test() {
  {
    using Array = int[1];
    Array array;
    std::same_as<Array*> auto result = std::construct_at(&array);
    assert(result == &array);
    assert(array[0] == 0);
  }
  {
    using Array = int[2];
    Array array;
    std::same_as<Array*> auto result = std::construct_at(&array);
    assert(result == &array);
    assert(array[0] == 0);
    assert(array[1] == 0);
  }
  {
    using Array = int[3];
    Array array;
    std::same_as<Array*> auto result = std::construct_at(&array);
    assert(result == &array);
    assert(array[0] == 0);
    assert(array[1] == 0);
    assert(array[2] == 0);
  }

  return true;
}

void test_construction_counts() {
  {
    using Array = CountInitializations[1];
    CountInitializations array[1];
    CountInitializations::constructions = 0;
    std::construct_at(&array);
    assert(CountInitializations::constructions == 1);
  }
  {
    using Array = CountInitializations[2];
    CountInitializations array[2];
    CountInitializations::constructions = 0;
    std::construct_at(&array);
    assert(CountInitializations::constructions == 2);
  }
  {
    using Array = CountInitializations[3];
    CountInitializations array[3];
    CountInitializations::constructions = 0;
    std::construct_at(&array);
    assert(CountInitializations::constructions == 3);
  }
}

int main(int, char**) {
  test();
  test_construction_counts();
  static_assert(test());
  return 0;
}
