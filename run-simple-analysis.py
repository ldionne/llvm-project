#!/usr/bin/env python3

import csv
import json
import pathlib
import sys

build_dir = pathlib.Path(sys.argv[1])

# Grab all benchmarks across all test files of all builds
all_benchmark_files = {} # file path => parsed JSON
for p in pathlib.Path('build').glob('*/libcxx/test/benchmarks/**/benchmark-result.json'):
    with p.open() as f:
        try:
            all_benchmark_files[p] = json.load(f)
        except json.JSONDecodeError:
            # Some files are invalid JSON, for example benchmarks that fail to run or run only partially.
            # Ignore those.
            pass

# Set of all the benchmarks across all the benchmark files
all_benchmarks = {bm['name'] for js in all_benchmark_files.values() for bm in js['benchmarks']}

# Output the results for the requested build as CSV
rows = []
for (p, js) in all_benchmark_files.items():
    if not p.is_relative_to(build_dir):
        continue

    for benchmark in js['benchmarks']:
        rows.append((benchmark['name'], benchmark['real_time']))

# Make sure to also include benchmarks that exist in other builds but not this one, with a value of N/A
missing_benchmarks = all_benchmarks - set(name for (name, time) in rows)
for bm in missing_benchmarks:
    rows.append((bm, 'N/A'))

rows = sorted(rows, key=lambda row: row[0]) # sort by benchmark names

csv.writer(sys.stdout, delimiter=';').writerows(rows)
