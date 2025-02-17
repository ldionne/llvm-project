#!/usr/bin/env zsh

set -ex

function git-repo-root() {
    git rev-parse --show-toplevel
}

VARIANT=${1}
shift

UMBRELLA_ROOT="$(git-repo-root)"
BUILD_DIR="${UMBRELLA_ROOT}/build/${VARIANT}"
INSTALL_DIR="${UMBRELLA_ROOT}/install/${VARIANT}"

export CXX=clang++
export CC=clang
${CXX} --version

rm -rf "${BUILD_DIR}" "${INSTALL_DIR}"
mkdir -p "${BUILD_DIR}" "${INSTALL_DIR}"
args=(${@})

case "${VARIANT}" in
libcxx)
    cmake -S "${UMBRELLA_ROOT}/runtimes" -B "${BUILD_DIR}" -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo    \
        -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}"                                                     \
        -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind"                                         \
        ${args[@]}
 ;;
libstdcxx)
    cmake -S "${UMBRELLA_ROOT}/runtimes" -B "${BUILD_DIR}" -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo    \
            -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind"                                         \
            -DBENCHMARK_USE_LIBSTDCXX=ON                                                                \
            -DLIBSTDCXX_PREFIX="/opt/homebrew/Cellar/gcc/14.2.0_1"                                      \
            -DLIBSTDCXX_TRIPLE="aarch64-apple-darwin23"                                                 \
            -DLIBCXX_TEST_CONFIG=stdlib-libstdc++.cfg.in                                                \
            -DLIBCXX_TEST_PARAMS="libstdcxx_install_prefix=/opt/homebrew/Cellar/gcc/14.2.0_1;libstdcxx_version=14;libstdcxx_triple=aarch64-apple-darwin23"
;;
*)
    echo "Unknown configuration ${VARIANT}"
;;
esac
