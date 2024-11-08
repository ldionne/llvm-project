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

template <class T>
static void BM_std_minmax(benchmark::State& state) {
  std::vector<T> vec(state.range(), 3);

  for (auto _ : state) {
    benchmark::DoNotOptimize(vec);
    benchmark::DoNotOptimize(std::ranges::minmax(vec));
  }
}
BENCHMARK(BM_std_minmax<char>)->DenseRange(1, 32)->Range(64, 1 << 15);
BENCHMARK(BM_std_minmax<short>)->DenseRange(1, 32)->Range(64, 1 << 15);
BENCHMARK(BM_std_minmax<int>)->DenseRange(1, 32)->Range(64, 1 << 15);
BENCHMARK(BM_std_minmax<long long>)->DenseRange(1, 32)->Range(64, 1 << 15);
BENCHMARK(BM_std_minmax<unsigned char>)->DenseRange(1, 32)->Range(64, 1 << 15);
BENCHMARK(BM_std_minmax<unsigned short>)->DenseRange(1, 32)->Range(64, 1 << 15);
BENCHMARK(BM_std_minmax<unsigned int>)->DenseRange(1, 32)->Range(64, 1 << 15);
BENCHMARK(BM_std_minmax<unsigned long long>)->DenseRange(1, 32)->Range(64, 1 << 15);

BENCHMARK_MAIN();
