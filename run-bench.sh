#!/usr/bin/env bash

# Make sure we fail early if something's wrong with the test setup
for b in libcxx system-libcxx libstdcxx; do
    if [[ -e build/${b} ]]; then
        cmake --build build/${b} --target cxx-test-depends
        find build/${b}/libcxx/test/benchmarks -name 'benchmark-result.json' | xargs rm
    fi
done

for b in libcxx system-libcxx libstdcxx; do
    if [[ -e build/${b} ]]; then
        ./libcxx/utils/libcxx-lit build/${b} -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/algorithms
        ./libcxx/utils/libcxx-lit build/${b} -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/containers/{associative,sequence,string.bench.cpp}
    fi
done
