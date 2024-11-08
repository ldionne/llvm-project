#!/usr/bin/env zsh

for version in llvm-trunk llvm-19 llvm-18 llvm-17 llvm-16 llvm-15; do
    ./benchmark-with-libcxx.sh ${version}
done
