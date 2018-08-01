//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// REQUIRES: verify-support
// UNSUPPORTED: c++98, c++03, c++11, c++14

#include <memory>


struct Public
    : public std::enable_shared_from_this<Public> // expected-no-warning
{ };

struct Protected
    // expected-warning@enable_shared_from_this.fail.cpp:* 1 {{TODO}}
    : protected std::enable_shared_from_this<Protected>
{ };

struct Private
    // expected-warning@enable_shared_from_this.fail.cpp:* 1 {{TODO}}
    : private std::enable_shared_from_this<Private>
{ };

int main() { }
