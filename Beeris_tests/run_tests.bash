#!/bin/bash

set -e

for infile in *.in; do
    base="${infile%.in}"
    outfile="$base.out"
    resfile="$base.res"

    if [[ ! -f "$outfile" ]]; then
        echo "Missing output file: $outfile"
        exit 1
    fi

    echo "Running test: $base"
    ./hw3 < "$infile" > "$resfile" 2>&1

    if ! diff -q "$resfile" "$outfile" > /dev/null; then
        echo "❌ Test $base failed. Difference:"
        diff "$resfile" "$outfile"
        exit 1
    else
        echo "✅ Test $base passed"
    fi
done

echo "🎉 All tests passed!"
