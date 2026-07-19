#!/bin/sh
# Regression test for wrong fastest-trace delay #2035819
set -eu

binary="$1"
model="$2"
query="$3"

out=$("$binary" --k-bound 16 --trace 2 --search-strategy default \
      --verification-method 0 --memory-optimization 1 --gcd-lower \
      "$model" "$query" 2>&1)

printf '%s\n' "$out" | grep -q 'Query is satisfied' || {
    echo "FAIL: query not satisfied"
    printf '%s\n' "$out"
    exit 1
}

sum=$(printf '%s\n' "$out" | awk -F'[<>]' '/<delay>/{s+=$3} END{print s+0}')
if [ "$sum" -ne 12 ]; then
    echo "FAIL: total trace delay is $sum, expected 12"
    printf '%s\n' "$out"
    exit 1
fi
echo "OK: total trace delay is 12"
