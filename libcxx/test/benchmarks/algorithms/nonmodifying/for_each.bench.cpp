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
#include <string>
#include <vector>

#include <benchmark/benchmark.h>

template <class Container, class Alg>
static void bm_for_each(benchmark::State& st, Alg for_each) {
  std::size_t const size = st.range();
  Container v(size, 1);
  auto first = v.begin();
  auto last  = v.end();

  for (auto _ : st) {
    auto result = for_each(first, last, [](int& x) { x = std::clamp(x, 10, 100); });
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}

int main(int argc, char** argv) {
  auto std_for_each    = [](auto first, auto last, auto f) { return std::for_each(first, last, f); };
  auto ranges_for_each = [](auto first, auto last, auto f) { return std::ranges::for_each(first, last, f); };

  auto bench = [](std::string name, auto f) {
    benchmark::RegisterBenchmark(name, f)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(64)->Arg(512)->Arg(1 << 20);
  };

  // std::for_each
  bench("std::for_each(vector-iter, vector-iter)", [&](auto& st) { bm_for_each<std::vector<int>>(st, std_for_each); });
  bench("std::for_each(deque-iter, deque-iter)", [&](auto& st) { bm_for_each<std::deque<int>>(st, std_for_each); });
  bench("std::for_each(list-iter, list-iter)", [&](auto& st) { bm_for_each<std::list<int>>(st, std_for_each); });

  // std::ranges::for_each
  bench("ranges::for_each(vector-iter, vector-iter)",
        [&](auto& st) { bm_for_each<std::vector<int>>(st, ranges_for_each); });
  bench("ranges::for_each(deque-iter, deque-iter)",
        [&](auto& st) { bm_for_each<std::deque<int>>(st, ranges_for_each); });
  bench("ranges::for_each(list-iter, list-iter)", [&](auto& st) { bm_for_each<std::list<int>>(st, ranges_for_each); });

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
