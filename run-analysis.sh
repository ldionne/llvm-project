#!/usr/bin/env zsh

ANALYSIS_DIR="${1}"
BASELINE="${ANALYSIS_DIR}/libstdcxx"
CANDIDATE="${ANALYSIS_DIR}/libcxx"
MONOREPO="${PWD}"

function compare() {
  benchmark="${1}"
  filter="${2}"
  ${MONOREPO}/third-party/benchmark/tools/compare.py benchmarksfiltered \
    $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${BASELINE}" "${benchmark}") "${filter}" \
    $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${CANDIDATE}" "${benchmark}") "${filter}"
}

# Analysis for the main vector<int> algorithms
for benchmark in ${MONOREPO}/libcxx/test/benchmarks/algorithms/**/*.bench.cpp; do
  if ! [[ -f $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${CANDIDATE}" "${benchmark}") ]]; then
    continue
  fi
  if ! [[ -f $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${BASELINE}" "${benchmark}") ]]; then
    continue
  fi
  geomean=$(compare "${benchmark}" "vector<int>" | grep "OVERALL_GEOMEAN" | cut -w -f 3)
  alg_name=$(basename "${benchmark}" | sed s/.bench.cpp//)
  echo "${alg_name}(vector<int>),${geomean}"
done

exit

# Analysis for algorithms with vector<bool> special-case
for algo in "modifying/copy_backward" "modifying/copy_n" "modifying/copy" "modifying/fill_n" "modifying/fill"; do
  compare "${MONOREPO}/libcxx/test/benchmarks/algorithms/${algo}.bench.cpp" "vector<bool>"
done

# Analysis for algorithms with deque special-case
for algorithm in ${MONOREPO}/libcxx/test/benchmarks/algorithms/**/*.bench.cpp; do
  if ! [[ -f $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${CANDIDATE}" "${algorithm}") ]]; then
    continue
  fi
  if ! [[ -f $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${BASELINE}" "${algorithm}") ]]; then
    continue
  fi
  compare "${algorithm}" "deque<int>"
done
