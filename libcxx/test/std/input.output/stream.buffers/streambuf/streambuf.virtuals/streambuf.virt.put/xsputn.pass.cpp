//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <streambuf>

// template <class charT, class traits = char_traits<charT> >
// class basic_streambuf;

// streamsize xsputn(const char_type* s, streamsize n);
// ADDITIONAL_COMPILE_FLAGS: -O0 -g
#include <streambuf>
#include <cassert>
#include <cstring>

#include "test_macros.h"

struct test : public std::basic_streambuf<char> {
  typedef std::basic_streambuf<char> base;

  test() {}
  explicit test(bool* tracker) : overflow_called(tracker) {}

  void setp(char* pbeg, char* pend) { base::setp(pbeg, pend); }

  using int_type    = std::basic_streambuf<char>::int_type;
  using traits_type = std::basic_streambuf<char>::traits_type;
  virtual int_type overflow(int_type c = traits_type::eof()) {
    if (overflow_called != nullptr)
      *overflow_called = true;
    return std::basic_streambuf<char>::overflow(c);
  }

  bool* overflow_called = nullptr;
};

int main(int, char**) {
  // Test outputing into a streambuf without any output buffer
  {
    test t;
    char in[] = "123456";
    assert(t.sputn(in, sizeof(in)) == 0);
  }

  // Test outputing into a streambuf with enough output buffer
  {
    test t;
    char in[]            = "123456";
    char out[sizeof(in)] = {0};
    t.setp(out, out + sizeof(out));
    assert(t.sputn(in, sizeof(in)) == sizeof(in));
    assert(std::strcmp(in, out) == 0);
  }

  // Try outputing into a streambuf that doesn't have enough output buffer to satisfy the request.
  // This will have to call overflow().
  {
    bool overflow_called = false;
    test t(&overflow_called);
    char in[]                = "123456";
    char out[sizeof(in) / 2] = {0};
    t.setp(out, out + sizeof(out));
    assert(t.sputn(in, sizeof(in)) == sizeof(out));
    assert(overflow_called);
  }

  return 0;
}
