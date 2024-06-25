//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14

// UNSUPPORTED: libcpp-has-no-incomplete-pstl

// <numeric>

// template<class ExecutionPolicy,
//          class ForwardIterator, class T,
//          class BinaryOperation, class UnaryOperation>
//   T transform_reduce(ExecutionPolicy&& exec,
//                      ForwardIterator first, ForwardIterator last,
//                      T init, BinaryOperation binary_op, UnaryOperation unary_op);

#include <numeric>
#include <string>
#include <vector>

#include "MoveOnly.h"
#include "test_execution_policies.h"
#include "test_iterators.h"
#include "test_macros.h"

template <class Iter, class ValueT>
struct Test {
  template <class Policy>
  void operator()(Policy&& policy) {
    for (const auto& pair : {std::pair{0, 34}, {1, 35}, {2, 37}, {100, 5084}, {350, 61459}}) {
      auto [size, expected] = pair;
      std::vector<ValueT> a(size);
      for (int i = 0; i != size; ++i)
        a[i] = ValueT(i);

      decltype(auto) ret = std::transform_reduce(
          policy,
          Iter(std::data(a)),
          Iter(std::data(a) + std::size(a)),
          ValueT(34),
          [check = std::string("Banane")](ValueT const& i, ValueT const& j) {
            assert(check == "Banane"); // ensure that no double-moves happen
            return i + j;
          },
          [check = std::string("Banane")](ValueT const& i) {
            assert(check == "Banane"); // ensure that no double-moves happen
            return i + 1;
          });
      static_assert(std::is_same_v<decltype(ret), ValueT>);
      assert(ret == expected);
    }
  }
};

int main(int, char**) {
  types::for_each(types::type_list<int, MoveOnly>{}, types::apply_type_identity{[](auto value_type) {
                    using ValueType = typename decltype(value_type)::type;
                    types::for_each(
                        types::forward_iterator_list<ValueType*>{}, types::apply_type_identity{[](auto iterator) {
                          using Iterator = typename decltype(iterator)::type;
                          test_execution_policies(Test<Iterator, ValueType>{});
                        }});
                  }});

  return 0;
}
