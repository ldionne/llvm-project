//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: no-rtti
// UNSUPPORTED: c++03

// Make sure that operator< on std::type_info is an ordering relation.
// We do this by defining a bunch of classes in different TUs with RTTIs
// having various names and visibilities (which can have an impact on how
// they are ordered depending on the ABI), and making sure that that set
// indeed ordered totally by operator<. We do this very naively based on
// the text book definition of a total order.

// RUN: %{cxx} %s %{flags} %{compile_flags} -c -o %t.tu1.o -DTU1
// RUN: %{cxx} %s %{flags} %{compile_flags} -c -o %t.tu2.o -DTU2
// RUN: %{cxx} %s %{flags} %{compile_flags} -c -o %t.main.o -DMAIN
// RUN: %{cxx} %t.tu1.o %t.tu2.o %t.main.o %{flags} %{link_flags} -o %t.exe
// RUN: %{exec} %t.exe

#include <cassert>
#include <typeindex>
#include <vector>

extern std::vector<std::type_index> registry;

void register1();
void register2();

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

struct __attribute__((visibility("default"))) KeyFunction {
  virtual ~KeyFunction();
};

struct __attribute__((visibility("default"))) NoKeyFunction {
  virtual void foo() { }
};

#if defined(TU1)

struct __attribute__((visibility("hidden"))) SameNameHidden { };

KeyFunction::~KeyFunction() { }

void register1() {
  registry.push_back(std::type_index(typeid(NoKeyFunction)));
  registry.push_back(std::type_index(typeid(KeyFunction)));
  registry.push_back(std::type_index(typeid(SameNameHidden)));
}

#elif defined(TU2)

struct __attribute__((visibility("hidden"))) SameNameHidden { };

void register2() {
  registry.push_back(std::type_index(typeid(NoKeyFunction)));
  registry.push_back(std::type_index(typeid(KeyFunction)));
  registry.push_back(std::type_index(typeid(SameNameHidden)));
}

#elif defined(MAIN)

std::vector<std::type_index> registry;

int main(int, char**) {
  register1();
  register2();

  check_total_order(registry);
  return 0;
}

#endif
