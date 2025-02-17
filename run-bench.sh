#!/usr/bin/env bash

# Make sure we fail early if something's wrong with the test setup
cmake --build build/libcxx --target cxx-test-depends
cmake --build build/libstdcxx --target cxx-test-depends

./libcxx/utils/libcxx-lit build/libcxx -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/algorithms
./libcxx/utils/libcxx-lit build/libstdcxx -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/algorithms

./libcxx/utils/libcxx-lit build/libcxx -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/containers/{associative,sequence,string.bench.cpp}
./libcxx/utils/libcxx-lit build/libstdcxx -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/containers/{associative,sequence,string.bench.cpp}
