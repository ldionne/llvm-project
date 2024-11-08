#!/usr/bin/env zsh

git checkout main
./build-libcxx.sh llvm-trunk

for version in 19 18 17 16 15 14; do
    git checkout release/${version}.x
    ./build-libcxx.sh llvm-${version}
done
