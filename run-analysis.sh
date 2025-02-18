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

function exists-in() {
  dataset=${1}
  benchmark=${2}
  if [[ -f $(${MONOREPO}/libcxx/utils/libcxx-benchmark-json "${dataset}" "${benchmark}") ]]; then
    return 0
  fi
  return 1
}

########################
# Analysis of containers
########################
for benchmark in ${MONOREPO}/libcxx/test/benchmarks/containers/**/*.bench.cpp; do
  if ! exists-in ${BASELINE} ${benchmark} || ! exists-in ${CANDIDATE} ${benchmark}; then
    continue
  fi
  # Discard sizes other than 8192 since they make the data harder to display
  results=$(compare-filter "${benchmark}" "/8192" | grep "vs. /8192" | sed -E 's%(.+/8192])\s+%%')
  echo ${results} | sed -E 's%(.+)\[/8192 vs. /8192][^-+]+([0-9.+-]+).*%\1;\2%' | prettify-final
done

##############################################
# Analysis for the legacy algorithm benchmarks
##############################################
for benchmark in libcxx/test/benchmarks/algorithms/*.bench.cpp; do
  if ! exists-in ${BASELINE} ${benchmark} || ! exists-in ${CANDIDATE} ${benchmark}; then
    continue
  fi
  # std::find is handled specially below
  if [[ "${benchmark}" == */find.bench.cpp ]]; then
    continue
  fi
  algo=$(basename ${benchmark} | sed 's/.bench.cpp//')
  compare ${benchmark} | extract-geomean "${algo}"
done

#######################################################################################
# Analysis for the main vector<int> algorithms (the ones that were re-written recently)
#######################################################################################
for benchmark in ${MONOREPO}/libcxx/test/benchmarks/algorithms/{modifying,nonmodifying,partitions}/*.bench.cpp; do
  if ! exists-in ${BASELINE} ${benchmark} || ! exists-in ${CANDIDATE} ${benchmark}; then
    continue
  fi
  algo=$(basename "${benchmark}" | sed s/.bench.cpp//)
  compare-filter "${benchmark}" "vector<int>|vector-iter" | extract-geomean "${algo}(vector<int>)" | prettify-final
done
compare-filter "${MONOREPO}/libcxx/test/benchmarks/algorithms/find.bench.cpp" "vector<int>" | extract-geomean "find(vector<int>)" | prettify-final
compare-filter "${MONOREPO}/libcxx/test/benchmarks/algorithms/find.bench.cpp" "vector<char>" | extract-geomean "find(vector<char>)" | prettify-final # This one's interesting cause we have a special case

########################################################
# Analysis for algorithms with vector<bool> special-case
########################################################
for algo in "modifying/copy_backward" "modifying/copy_n" "modifying/copy" "modifying/fill_n" "modifying/fill"; do
  compare-filter "${MONOREPO}/libcxx/test/benchmarks/algorithms/${algo}.bench.cpp" "vector<bool>" | extract-geomean "$(basename ${algo})(vector<bool>)" | prettify-final
done
compare-filter "${MONOREPO}/libcxx/test/benchmarks/algorithms/find.bench.cpp" "vector_bool" | extract-geomean "find(vector<bool>)" | prettify-final

########################################################################################
# Analysis for algorithms with deque special-case (only algorithms that were re-written)
########################################################################################
for benchmark in ${MONOREPO}/libcxx/test/benchmarks/algorithms/{modifying,nonmodifying,partitions}/*.bench.cpp; do
  if ! exists-in ${BASELINE} ${benchmark} || ! exists-in ${CANDIDATE} ${benchmark}; then
    continue
  fi
  algo=$(basename "${benchmark}" | sed s/.bench.cpp//)
  compare-filter "${benchmark}" "deque<int>|deque-iter" | extract-geomean "${algo}(deque<int>)" | prettify-final
done
compare-filter "${MONOREPO}/libcxx/test/benchmarks/algorithms/find.bench.cpp" "deque<int>" | extract-geomean "find(deque<int>)"
