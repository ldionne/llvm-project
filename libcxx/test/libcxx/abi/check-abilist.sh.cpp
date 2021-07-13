//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// This test compares the symbols exported by the shared library under test
// against a pre-generated list of symbols that are expected to be exported.
// It fails if they do not match.
//
// This allows catching bugs where we stop exporting a symbol, or where we
// unknowingly start exporting one.
//
// If this test fails, it is probably because you have added or removed symbols
// from the shared library. If that is intended, you'll need to re-generate the
// symbols lists for all relevant configurations we support. You can do so by
// building the `libcxx-generate-files` target on all appropriate configurations,
// or by downloading the lists from your failed CI run. Go to your failed build,
// select the `artifacts` tab and download the relevant lists.

// REQUIRES: has-abi-list

// RUN: %{python} %{libcxx_src_dir}/utils/sym_diff.py
// RUN:     --only-stdlib-symbols --strict \
// RUN:     %{abi-list-file} \
// RUN:     <path-to-shared-library>
