//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

#include <algorithm>
#include <cstddef>
#include <deque>
#include <iterator>
#include <list>
#include <vector>

#include <benchmark/benchmark.h>

template <class Container>
static void bm_contains_early_out(benchmark::State& st) {
  std::size_t const size = st.range();
  Container v(size, 1);
  *std::next(v.begin(), size / 10) = 42; // bail out after checking 10% of values
  auto first                       = v.begin();
  auto last                        = v.end();

  for (auto _ : st) {
    auto result = std::ranges::contains(first, last, 42);
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}

template <class Container>
static void bm_contains_no_early_exit(benchmark::State& st) {
  std::size_t const size = st.range();
  Container v(size, 1);
  auto first = v.begin();
  auto last  = v.end();

  for (auto _ : st) {
    auto result = std::ranges::contains(first, last, 42);
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}

int main(int argc, char** argv) {
  auto bench = [](std::string name, auto f) {
    benchmark::RegisterBenchmark(name, f)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(64)->Arg(512)->Arg(1 << 20);
  };

  bench("ranges::contains(vector-iter, vector-iter) (early out)", bm_contains_early_out<std::vector<int>>);
  bench("ranges::contains(vector-iter, vector-iter) (process all)", bm_contains_no_early_exit<std::vector<int>>);

  bench("ranges::contains(deque-iter, deque-iter) (early out)", bm_contains_early_out<std::deque<int>>);
  bench("ranges::contains(deque-iter, deque-iter) (process all)", bm_contains_no_early_exit<std::deque<int>>);

  bench("ranges::contains(list-iter, list-iter) (early out)", bm_contains_early_out<std::list<int>>);
  bench("ranges::contains(list-iter, list-iter) (process all)", bm_contains_no_early_exit<std::list<int>>);

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
