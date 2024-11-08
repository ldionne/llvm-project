#!/usr/bin/env zsh

set -x

function git-repo-root() {
    git rev-parse --show-toplevel
}

UMBRELLA_ROOT="$(git-repo-root)"
BUILD_DIR="${UMBRELLA_ROOT}/build/libstdcxx"
INSTALL_DIR="${UMBRELLA_ROOT}/install/libstdcxx"

export CXX=clang++
export CC=clang
${CXX} --version

rm -rf "${BUILD_DIR}" "${INSTALL_DIR}"
mkdir -p "${BUILD_DIR}" "${INSTALL_DIR}"
args=(${@})

cmake -S "${UMBRELLA_ROOT}/runtimes" -B "${BUILD_DIR}" -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo    \
        -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}"                                                     \
        -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind"                                         \
        -DBENCHMARK_USE_LIBSTDCXX=ON                                                                \
        -DLIBCXX_TEST_CONFIG=stdlib-libstdc++.cfg.in                                                \
        ${args[@]}

ninja -C "${BUILD_DIR}" cxx-test-depends

./libcxx/utils/libcxx-lit "${BUILD_DIR}" --show-all -j1 --time-tests                                        \
                                         --param optimization=speed                                         \
                                         --param libstdcxx-install-prefix=/opt/homebrew/Cellar/gcc/14.1.0_1 \
                                         --param libstdcxx-version=14                                       \
                                         --param libstdcxx-triple=aarch64-apple-darwin23                    \
                                         libcxx/test/benchmarks/{containers,algorithms}

# Aggregate all the benchmark results into a common location
mkdir -p ${BUILD_DIR}/benchmark-results
for file in $(find "${BUILD_DIR}/libcxx/test/benchmarks" -name 'benchmark-result.json'); do
    bench=$(echo $file | grep --only-matching -E 'Output/(.+).bench.cpp' | sed 's_Output/__' | sed 's/.bench.cpp//')
    cp ${file} ${BUILD_DIR}/benchmark-results/${bench}.json
done
