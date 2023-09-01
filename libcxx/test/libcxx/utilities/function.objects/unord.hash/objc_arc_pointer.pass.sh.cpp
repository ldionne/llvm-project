//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// RUN: %cxx -c %s %flags %compile_flags -fobjc-arc -ObjC++
// UNSUPPORTED: c++98, c++03

// Test that we can use Objective-C ARC pointers with unordered_set.
//
// TODO: Also test for other unordered containers.

#import <Foundation/Foundation.h>
#import <unordered_set>


int main(int, char**) {
    {
        using Set = std::unordered_set<NSString*>;
        Set set;
        NSString* x = [NSString stringWithUTF8String:"hello"];
        NSString* y = [NSString stringWithUTF8String:"world"];
        set.insert(x);
        set.insert(y);
    }
    {
        using Set = std::unordered_set<id>;
        Set set;
        NSString* x = [NSString stringWithUTF8String:"hello"];
        NSString* y = [NSString stringWithUTF8String:"world"];
        set.insert(x);
        set.insert(y);
    }

    return 0;
}
