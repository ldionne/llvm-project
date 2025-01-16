#!/usr/bin/env zsh

set -x

function git-repo-root() {
    git rev-parse --show-toplevel
}

UMBRELLA_ROOT="$(git-repo-root)"

export CXX=clang++
export CC=clang
${CXX} --version

# ${UMBRELLA_ROOT}/install/${version} must already contain pre-built content for older releases
version=${1}

BUILD_DIR="${UMBRELLA_ROOT}/build/${version}"
INSTALL_DIR="${UMBRELLA_ROOT}/install/${version}"
rm -rf "${BUILD_DIR}" "${INSTALL_DIR}"
mkdir -p "${BUILD_DIR}" "${INSTALL_DIR}"

# Prepare the test suite
cmake -S "${UMBRELLA_ROOT}/runtimes" -B "${BUILD_DIR}" -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo    \
        -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}"                                                     \
        -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi"                                                   \
        -DLIBCXXABI_USE_LLVM_UNWINDER=OFF

ninja -C "${BUILD_DIR}" install-cxx install-cxxabi
ninja -C "${BUILD_DIR}" cxx-test-depends

# Run the benchmarks
./libcxx/utils/libcxx-lit "${BUILD_DIR}"                            \
    --show-all -j1 --time-tests                                     \
    --param "prefix=${UMBRELLA_ROOT}/install/${version}"            \
    --param optimization=speed                                      \
    libcxx/test/benchmarks/{containers,algorithms}

# Aggregate all the benchmark results into a common location
mkdir -p ${BUILD_DIR}/benchmark-results
for file in $(find "${BUILD_DIR}/libcxx/test/benchmarks" -name 'benchmark-result.json'); do
    bench=$(echo $file | grep --only-matching -E 'Output/(.+).bench.cpp' | sed 's_Output/__' | sed 's/.bench.cpp//')
    cp ${file} ${BUILD_DIR}/benchmark-results/${bench}.json
done
