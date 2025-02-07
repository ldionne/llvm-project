//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

#include <algorithm>
#include <cstddef>
#include <deque>
#include <list>
#include <numeric>
#include <string>
#include <vector>

#include <benchmark/benchmark.h>

template <class Container, class Alg>
static void bm_none_of_early_out(benchmark::State& st, Alg none_of) {
  std::size_t const size = st.range();
  Container v(size);
  std::iota(v.begin(), v.end(), 0);
  auto first          = v.begin();
  auto last           = v.end();
  int const threshold = size / 10; // bail out after the first 10% of values

  for (auto _ : st) {
    auto result = none_of(first, last, [&](int x) { return x > threshold; });
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}

template <class Container, class Alg>
static void bm_none_of_no_early_exit(benchmark::State& st, Alg none_of) {
  std::size_t const size = st.range();
  Container v(size, 2); // only even numbers
  auto first = v.begin();
  auto last  = v.end();

  for (auto _ : st) {
    auto result = none_of(first, last, [](int x) { return x % 2 != 0; });
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}

int main(int argc, char** argv) {
  auto std_none_of    = [](auto first, auto last, auto pred) { return std::none_of(first, last, pred); };
  auto ranges_none_of = [](auto first, auto last, auto pred) { return std::ranges::none_of(first, last, pred); };

  auto bench = [](std::string name, auto f) {
    benchmark::RegisterBenchmark(name, f)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(64)->Arg(512)->Arg(1 << 20);
  };

  // std::none_of
  bench("std::none_of(vector-iter, vector-iter) (early out)",
        [&](auto& st) { bm_none_of_early_out<std::vector<int>>(st, std_none_of); });
  bench("std::none_of(vector-iter, vector-iter) (process all)",
        [&](auto& st) { bm_none_of_no_early_exit<std::vector<int>>(st, std_none_of); });

  bench("std::none_of(deque-iter, deque-iter) (early out)",
        [&](auto& st) { bm_none_of_early_out<std::deque<int>>(st, std_none_of); });
  bench("std::none_of(deque-iter, deque-iter) (process all)",
        [&](auto& st) { bm_none_of_no_early_exit<std::deque<int>>(st, std_none_of); });

  bench("std::none_of(list-iter, list-iter) (early out)",
        [&](auto& st) { bm_none_of_early_out<std::list<int>>(st, std_none_of); });
  bench("std::none_of(list-iter, list-iter) (process all)",
        [&](auto& st) { bm_none_of_no_early_exit<std::list<int>>(st, std_none_of); });

  // std::ranges::none_of
  bench("ranges::none_of(vector-iter, vector-iter) (early out)",
        [&](auto& st) { bm_none_of_early_out<std::vector<int>>(st, ranges_none_of); });
  bench("ranges::none_of(vector-iter, vector-iter) (process all)",
        [&](auto& st) { bm_none_of_no_early_exit<std::vector<int>>(st, ranges_none_of); });

  bench("ranges::none_of(list-iter, list-iter) (early out)",
        [&](auto& st) { bm_none_of_early_out<std::list<int>>(st, ranges_none_of); });
  bench("ranges::none_of(list-iter, list-iter) (process all)",
        [&](auto& st) { bm_none_of_no_early_exit<std::list<int>>(st, ranges_none_of); });

  bench("ranges::none_of(list-iter, list-iter) (early out)",
        [&](auto& st) { bm_none_of_early_out<std::list<int>>(st, ranges_none_of); });
  bench("ranges::none_of(list-iter, list-iter) (process all)",
        [&](auto& st) { bm_none_of_no_early_exit<std::list<int>>(st, ranges_none_of); });

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
