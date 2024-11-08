//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

#include <algorithm>
#include <cassert>

#include <benchmark/benchmark.h>
#include "test_macros.h"

template <class T>
static void BM_std_min(benchmark::State& state) {
  std::vector<T> vec(state.range(), 3);

  for (auto _ : state) {
    benchmark::DoNotOptimize(vec);
    benchmark::DoNotOptimize(std::ranges::min(vec));
  }
}
BENCHMARK(BM_std_min<char>)->DenseRange(1, 32)->Range(64, 1 << 15);
BENCHMARK(BM_std_min<short>)->DenseRange(1, 32)->Range(64, 1 << 15);
BENCHMARK(BM_std_min<int>)->DenseRange(1, 32)->Range(64, 1 << 15);
BENCHMARK(BM_std_min<long long>)->DenseRange(1, 32)->Range(64, 1 << 15);
#ifndef TEST_HAS_NO_INT128
BENCHMARK(BM_std_min<__int128>)->DenseRange(1, 32)->Range(64, 1 << 15);
#endif
BENCHMARK(BM_std_min<unsigned char>)->DenseRange(1, 32)->Range(64, 1 << 15);
BENCHMARK(BM_std_min<unsigned short>)->DenseRange(1, 32)->Range(64, 1 << 15);
BENCHMARK(BM_std_min<unsigned int>)->DenseRange(1, 32)->Range(64, 1 << 15);
BENCHMARK(BM_std_min<unsigned long long>)->DenseRange(1, 32)->Range(64, 1 << 15);
#ifndef TEST_HAS_NO_INT128
BENCHMARK(BM_std_min<unsigned __int128>)->DenseRange(1, 32)->Range(64, 1 << 15);
#endif

BENCHMARK_MAIN();
