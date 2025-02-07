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
static void bm_all_of_early_out(benchmark::State& st, Alg all_of) {
  std::size_t const size = st.range();
  Container v(size);
  std::iota(v.begin(), v.end(), 0);
  auto first          = v.begin();
  auto last           = v.end();
  int const threshold = size / 10; // bail out after the first 10% of values

  for (auto _ : st) {
    auto result = all_of(first, last, [&](int x) { return x < threshold; });
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}

template <class Container, class Alg>
static void bm_all_of_no_early_exit(benchmark::State& st, Alg all_of) {
  std::size_t const size = st.range();
  Container v(size, 2); // only even numbers
  auto first = v.begin();
  auto last  = v.end();

  for (auto _ : st) {
    auto result = all_of(first, last, [](int x) { return x % 2 == 0; });
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}

int main(int argc, char** argv) {
  auto std_all_of    = [](auto first, auto last, auto pred) { return std::all_of(first, last, pred); };
  auto ranges_all_of = [](auto first, auto last, auto pred) { return std::ranges::all_of(first, last, pred); };

  auto bench = [](std::string name, auto f) {
    benchmark::RegisterBenchmark(name, f)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(64)->Arg(512)->Arg(1 << 20);
  };

  // std::all_of
  bench("std::all_of(vector-iter, vector-iter) (early out)",
        [&](auto& st) { bm_all_of_early_out<std::vector<int>>(st, std_all_of); });
  bench("std::all_of(vector-iter, vector-iter) (process all)",
        [&](auto& st) { bm_all_of_no_early_exit<std::vector<int>>(st, std_all_of); });

  bench("std::all_of(deque-iter, deque-iter) (early out)",
        [&](auto& st) { bm_all_of_early_out<std::deque<int>>(st, std_all_of); });
  bench("std::all_of(deque-iter, deque-iter) (process all)",
        [&](auto& st) { bm_all_of_no_early_exit<std::deque<int>>(st, std_all_of); });

  bench("std::all_of(list-iter, list-iter) (early out)",
        [&](auto& st) { bm_all_of_early_out<std::list<int>>(st, std_all_of); });
  bench("std::all_of(list-iter, list-iter) (process all)",
        [&](auto& st) { bm_all_of_no_early_exit<std::list<int>>(st, std_all_of); });

  // std::ranges::all_of
  bench("ranges::all_of(vector-iter, vector-iter) (early out)",
        [&](auto& st) { bm_all_of_early_out<std::vector<int>>(st, ranges_all_of); });
  bench("ranges::all_of(vector-iter, vector-iter) (process all)",
        [&](auto& st) { bm_all_of_no_early_exit<std::vector<int>>(st, ranges_all_of); });

  bench("ranges::all_of(list-iter, list-iter) (early out)",
        [&](auto& st) { bm_all_of_early_out<std::list<int>>(st, ranges_all_of); });
  bench("ranges::all_of(list-iter, list-iter) (process all)",
        [&](auto& st) { bm_all_of_no_early_exit<std::list<int>>(st, ranges_all_of); });

  bench("ranges::all_of(list-iter, list-iter) (early out)",
        [&](auto& st) { bm_all_of_early_out<std::list<int>>(st, ranges_all_of); });
  bench("ranges::all_of(list-iter, list-iter) (process all)",
        [&](auto& st) { bm_all_of_no_early_exit<std::list<int>>(st, ranges_all_of); });

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
