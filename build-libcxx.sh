#!/usr/bin/env zsh

set -ex

function git-repo-root() {
    git rev-parse --show-toplevel
}

release=${1}

UMBRELLA_ROOT="$(git-repo-root)"
BUILD_DIR="${UMBRELLA_ROOT}/build"
INSTALL_DIR="${UMBRELLA_ROOT}/install/${release}"

export CXX=clang++
export CC=clang
${CXX} --version

rm -rf "${BUILD_DIR}" "${INSTALL_DIR}"
mkdir -p "${BUILD_DIR}" "${INSTALL_DIR}"

cmake -S "${UMBRELLA_ROOT}/runtimes" -B "${BUILD_DIR}" -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo    \
        -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}"                                                     \
        -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi"                                                   \
        -DLIBCXXABI_USE_LLVM_UNWINDER=OFF                                                           \
        ${args[@]}
ninja -C "${BUILD_DIR}" install-cxx install-cxxabi
