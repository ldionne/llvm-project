#!/usr/bin/env bash

caffeinate -dis ./libcxx/utils/libcxx-lit build/libcxx -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/algorithms
caffeinate -dis ./libcxx/utils/libcxx-lit build/libstdcxx -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/algorithms

caffeinate -dis ./libcxx/utils/libcxx-lit build/libcxx -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/containers/{associative,sequence}
caffeinate -dis ./libcxx/utils/libcxx-lit build/libstdcxx -sv --show-all --time-tests --param optimization=speed -j1 libcxx/test/benchmarks/containers/{associative,sequence}
