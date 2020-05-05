//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// XFAIL: libcpp-has-no-stdout
// UNSUPPORTED: c++98, c++03

// <iostream>

// wostream wcout;

// This test attempts to create a data race around the use of output streams
// with the hope that TSan will find it.

#include <iostream>
#include <thread>


void function() {
    std::wcout << L"hello" << std::endl;
}

int main(int, char**) {
    std::thread t1{function};
    std::thread t2{function};

    t1.join();
    t2.join();

    return 0;
}
