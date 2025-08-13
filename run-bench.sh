#!/usr/bin/env zsh

# Make sure we fail early if something's wrong with the test setup
for lib in libcxx system-libcxx libstdcxx; do
    if [[ -e build/${lib} ]]; then
        cmake --build build/${lib} --target cxx-test-depends
        find build/${lib}/libcxx/test/benchmarks -name 'benchmark-result.json' | xargs rm
    fi
done

# Run the actual benchmarks
for lib in libcxx system-libcxx libstdcxx; do
    if [[ -e build/${lib} ]]; then
        ./libcxx/utils/libcxx-lit build/${lib} -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/algorithms
        ./libcxx/utils/libcxx-lit build/${lib} -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/containers/{associative,sequence,string.bench.cpp}
    fi
done
