#!/usr/bin/env zsh

# Output the results in a form that can be easily imported as CSV
all_files=$(find build/{libcxx,system-libcxx,libstdcxx}/libcxx/test/benchmarks -name 'benchmark-result.json' 2>/dev/null)
benchmarks=$(jq '.benchmarks[].name' ${=all_files} | sort | uniq)

for lib in libcxx system-libcxx libstdcxx; do
    if [[ ! -e build/${lib} ]]; then
        continue
    fi

    echo
    echo "RESULTS FOR ${lib}"
    echo "======================================="
    files=$(find build/${lib}/libcxx/test/benchmarks -name 'benchmark-result.json')
    for bench in ${(f)benchmarks}; do
    result=$(jq ".benchmarks[] | select(.name == ${bench}) | .real_time" ${=files})
    if [[ ${result} == "" ]]; then
        result="N/A"
    fi
    echo "${bench};${result}"
    done
done
