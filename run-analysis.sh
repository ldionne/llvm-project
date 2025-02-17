#!/usr/bin/env zsh

ANALYSIS_DIR="${1}"
BASELINE="${ANALYSIS_DIR}/libstdcxx"
CANDIDATE="${ANALYSIS_DIR}/libcxx"
MONOREPO="${PWD}"

function compare-filter() {
  benchmark="${1}"
  filter="${2}"
  ${MONOREPO}/third-party/benchmark/tools/compare.py --no-color benchmarksfiltered \
    $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${BASELINE}" "${benchmark}") "${filter}" \
    $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${CANDIDATE}" "${benchmark}") "${filter}"
}

function compare() {
  benchmark="${1}"
  ${MONOREPO}/third-party/benchmark/tools/compare.py --no-color benchmarks \
    $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${BASELINE}" "${benchmark}") \
    $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${CANDIDATE}" "${benchmark}")
}

function extract-geomean() {
  algo="${1}"
  geomean=$(grep "OVERALL_GEOMEAN" | cut -w -f 2)
  echo "${algo};${geomean}"
}

function prettify-final() {
  sed 's/std:://g' | sed 's/<int, int>/<int>/' | sed 's/input-iter/it/g' | sed 's/iterator/it/g'
}

# Analysis of containers
for benchmark in ${MONOREPO}/libcxx/test/benchmarks/containers/**/*.bench.cpp; do
if ! [[ -f $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${CANDIDATE}" "${benchmark}") ]]; then
    continue
  fi
  if ! [[ -f $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${BASELINE}" "${benchmark}") ]]; then
    continue
  fi

  # Discard sizes other than 8192 since they make the data harder to display
  results=$(compare-filter "${benchmark}" "/8192" | grep "vs. /8192" | sed -E 's%(.+/8192])\s+%%')
  echo ${results} | sed -E 's%(.+)\[/8192 vs. /8192][^-+]+([0-9.+-]+).*%\1;\2%' | prettify-final
done

# Analysis for the main vector<int> algorithms
for benchmark in ${MONOREPO}/libcxx/test/benchmarks/algorithms/**/*.bench.cpp; do
  if ! [[ -f $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${CANDIDATE}" "${benchmark}") ]]; then
    continue
  fi
  if ! [[ -f $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${BASELINE}" "${benchmark}") ]]; then
    continue
  fi
  algo=$(basename "${benchmark}" | sed s/.bench.cpp//)
  compare-filter "${benchmark}" "vector<int>|vector-iter" | extract-geomean "${algo}(vector<int>)" | prettify-final
done

# Analysis for algorithms with vector<bool> special-case
for algo in "modifying/copy_backward" "modifying/copy_n" "modifying/copy" "modifying/fill_n" "modifying/fill"; do
  compare-filter "${MONOREPO}/libcxx/test/benchmarks/algorithms/${algo}.bench.cpp" "vector<bool>" | extract-geomean "$(basename ${algo})(vector<bool>)"
done

# Analysis for algorithms with deque special-case
for benchmark in ${MONOREPO}/libcxx/test/benchmarks/algorithms/**/*.bench.cpp; do
  if ! [[ -f $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${CANDIDATE}" "${benchmark}") ]]; then
    continue
  fi
  if ! [[ -f $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${BASELINE}" "${benchmark}") ]]; then
    continue
  fi
  algo=$(basename "${benchmark}" | sed s/.bench.cpp//)
  compare-filter "${benchmark}" "deque<int>|deque-iter" | extract-geomean "${algo}(deque<int>)" | prettify-final
done
