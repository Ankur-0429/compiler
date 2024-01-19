#!/bin/bash

hy_files=("tests/precedence/cases/test1.hy")
expected_exit_codes=(32)

# Loop through each file
for ((i=0; i<${#hy_files[@]}; i++)); do
    file="${hy_files[i]}"
    expected_exit_code="${expected_exit_codes[i]}"

    echo "Running test for $file"

    cmake-build-debug/hydro "$file" ./test

    actual_exit_code=$?

    if [ "$actual_exit_code" != "0" ]; then
          echo "Test failed: code didn't compile correct under test file: " + file
          exit 1
    fi

    ./test

    actual_exit_code=$?

    if [ "$actual_exit_code" -eq "$expected_exit_code" ]; then
        echo "Test passed (exit code $expected_exit_code)"
    else
        echo "Test failed! (expected exit code $expected_exit_code, actual exit code $actual_exit_code)"
        exit 1
    fi
done

exit 0