//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// TODO: Requires -fobjc-arc

// RUN: %{cxx} -c %s %{flags} %{compile_flags} -fobjc-arc -ObjC++
// UNSUPPORTED: c++03

// Test that we can use `std::hash` with Objective-C ARC pointers.

#import <Foundation/Foundation.h>
#import <functional>
#include <string>

int main(int, char**) {
    std::hash<id>()((id)nil);
    std::hash<NSString*>()(nil);
    std::hash<std::string*>()(nil);

    return 0;
}
