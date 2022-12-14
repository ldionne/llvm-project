//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: no-rtti
// UNSUPPORTED: c++03
// UNSUPPORTED: cant-build-shared-library

// Make sure that operator< on std::type_info is an ordering relation.
// We do this by defining a bunch of classes in different TUs with RTTIs
// having various names and visibilities (which can have an impact on how
// they are ordered depending on the ABI), and making sure that that set
// indeed ordered totally by operator<. We do this very naively based on
// the text book definition of a total order.
//
// In particular, the failure case we're trying to reproduce is:
//    non-unique C < unique B < non-unique A < non-unique C
//
// We have three types of classes here:
// 1. A_NonUnique:
//    This has no key function, which means that the vtable is weak, which in turn
//    means that it ends up with the non-unique RTTI bit set in the arm ABI.
//
// 2. B_Unique:
//    TODO
//
// 3. C_NonUnique:
//    TODO
//

// RUN: %{cxx} %{flags} %{compile_flags} %s %{link_flags} -shared -o %t.tu1.lib -DTU1
// RUN: %{cxx} %{flags} %{compile_flags} %s %{link_flags} -shared -o %t.tu2.lib -DTU2
// RUN: cd %T && %{cxx} %{flags} %{compile_flags} %s %{link_flags} ./%basename_t.tmp.tu1.lib ./%basename_t.tmp.tu2.lib -o %t.exe -DMAIN
// RUN: %{exec} %t.exe

#include <cassert>
#include <typeindex>
#include <vector>

#include <iostream>

void register1(std::vector<std::type_index>& registry);
void register2(std::vector<std::type_index>& registry);

// has key function -> RTTI is unique
struct __attribute__((visibility("default"))) B_Unique {
  virtual ~B_Unique();
};

#if defined(TU1)

struct __attribute__((visibility("default"))) A_NonUnique {
  virtual ~A_NonUnique() { }
};

// no key function -> vtable is weak -> ends up with non-unique RTTI bit set
// We make it hidden so we can have two such types with different RTTIs in the overall program.
struct __attribute__((visibility("hidden"))) C_NonUnique {
  virtual ~C_NonUnique() { }
};

// The position of this key function definition is important: it ensures that B's RTTI
// address is larger than C_NonUnique in this translation unit.
B_Unique::~B_Unique() { }

void register1(std::vector<std::type_index>& registry) {
  registry.push_back(std::type_index(typeid(C_NonUnique)));
  registry.push_back(std::type_index(typeid(A_NonUnique)));
}

#elif defined(TU2)

struct __attribute__((visibility("hidden"))) C_NonUnique {
  virtual ~C_NonUnique() { }
};

struct __attribute__((visibility("default"))) A_NonUnique {
  virtual ~A_NonUnique() { }
};

void register2(std::vector<std::type_index>& registry) {
  registry.push_back(std::type_index(typeid(A_NonUnique)));
  registry.push_back(std::type_index(typeid(C_NonUnique)));
}

#elif defined(MAIN)

template <typename Container>
void check_total_order(Container const& elements) {
  for (auto const& a : elements) {
    for (auto const& b: elements) {
      for (auto const& c: elements) {
        // Antisymmetry
        if (a <= b && b <= a)
          assert(a == b);

        // Transitivity
        if (a <= b && b <= c)
          assert(a <= c);

        // Connexivity
        assert(a <= b || b <= a);
      }
    }
  }
}

int main(int, char**) {
  std::vector<std::type_index> registry;
  register1(registry);
  register2(registry);
  registry.push_back(std::type_index(typeid(B_Unique)));

  check_total_order(registry);

  for (std::type_index idx : registry) {
    std::cout << idx.name() << " at " << idx.hash_code() << std::endl;
  }
  return 0;
}

#endif // defined(MAIN)
