#!/usr/bin/env zsh

BUILD_DIR="${1}"

# Make sure we fail early if something's wrong with the test setup
cmake --build ${BUILD_DIR} --target cxx-test-depends

# Run the actual benchmarks
./libcxx/utils/libcxx-lit ${BUILD_DIR} -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/algorithms
./libcxx/utils/libcxx-lit ${BUILD_DIR} -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/{streams,numeric,locale}
./libcxx/utils/libcxx-lit ${BUILD_DIR} -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/containers/{associative,sequence,string.bench.cpp}
