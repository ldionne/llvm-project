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
static void bm_contains_subrange_early_out(benchmark::State& st) {
  std::size_t const size = st.range();
  Container v(size, 1);
  Container subrange(size / 50, 42); // subrange of length 2% of the full range

  // At 10% of the range, put the subrange we're going to find
  std::ranges::copy(subrange, std::next(v.begin(), v.size() / 10));

  for (auto _ : st) {
    auto result = std::ranges::contains_subrange(v, subrange);
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(subrange);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}

template <class Container>
static void bm_contains_subrange_no_early_exit(benchmark::State& st) {
  std::size_t const size = st.range();
  Container v(size, 1);
  Container subrange(size / 50, 42); // subrange of length 2% of the full range, but we'll never find it

  for (auto _ : st) {
    auto result = std::ranges::contains_subrange(v, subrange);
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(subrange);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}

int main(int argc, char** argv) {
  auto bench = [](std::string name, auto f) {
    benchmark::RegisterBenchmark(name, f)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(64)->Arg(512)->Arg(1 << 20);
  };

  bench("ranges::contains_subrange(vector-iter, vector-iter) (early out)",
        bm_contains_subrange_early_out<std::vector<int>>);
  bench("ranges::contains_subrange(vector-iter, vector-iter) (process all)",
        bm_contains_subrange_no_early_exit<std::vector<int>>);

  bench("ranges::contains_subrange(deque-iter, deque-iter) (early out)",
        bm_contains_subrange_early_out<std::deque<int>>);
  bench("ranges::contains_subrange(deque-iter, deque-iter) (process all)",
        bm_contains_subrange_no_early_exit<std::deque<int>>);

  bench("ranges::contains_subrange(list-iter, list-iter) (early out)", bm_contains_subrange_early_out<std::list<int>>);
  bench("ranges::contains_subrange(list-iter, list-iter) (process all)",
        bm_contains_subrange_no_early_exit<std::list<int>>);

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
