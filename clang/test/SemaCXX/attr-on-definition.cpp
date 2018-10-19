// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -emit-llvm %s -o - | FileCheck %s

// https://github.com/llvm/llvm-project/issues/38727
//
// Make sure that attributes applied to definitions are applied even when the
// primary declaration does not contain the attribute. This only applies to
// non-template members of class templates.

#define HIDDEN __attribute__((visibility("hidden")))

template <typename T>
struct A {
    static int a;
    static void b();
    void c();
};

// CHECK: @_ZN1AIiE1aE = weak_odr hidden global
template <typename T> HIDDEN int A<T>::a;
template int A<int>::a;

// CHECK: define weak_odr hidden void @_ZN1AIiE1bEv
template <typename T> HIDDEN void A<T>::b() { }
template void A<int>::b();

// CHECK: define weak_odr hidden void @_ZN1AIiE1cEv
template <typename T> HIDDEN void A<T>::c() { }
template void A<int>::c();
