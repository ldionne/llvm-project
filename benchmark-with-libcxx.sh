#!/usr/bin/env zsh

set -x

function git-repo-root() {
    git rev-parse --show-toplevel
}

UMBRELLA_ROOT="$(git-repo-root)"
BUILD_DIR="${UMBRELLA_ROOT}/build/libcxx"

export CXX=clang++
export CC=clang
${CXX} --version

rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"

# Prepare the test suite
cmake -S "${UMBRELLA_ROOT}/runtimes" -B "${BUILD_DIR}" -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo    \
        -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi"                                                   \
        -DLIBCXXABI_USE_LLVM_UNWINDER=OFF
ninja -C "${BUILD_DIR}" cxx-test-depends

# Run the benchmarks
./libcxx/utils/libcxx-lit "${BUILD_DIR}"                            \
    --show-all -j1 --time-tests                                     \
    --param optimization=speed                                      \
    libcxx/test/benchmarks/containers/{vector,deque}.bench.cpp
