#!/bin/bash

./clearTestResults

TOTAL_FAILED=0
TOTAL_PASSED=0
TOTAL_SEGFAULTS=0
ALL_FAILED_TESTS=()
ALL_SEGFAULT_TESTS=()
FOLDER_RESULTS=()

# Process all subdirectories in allTests
for dir in allTests/*/; do
    dir_name=$(basename "$dir")
    echo "==============================================="
    echo "Testing directory: $dir_name"
    echo "==============================================="

    FAILED_TESTS=()
    SEGFAULT_TESTS=()
    DIR_TOTAL=0
    DIR_PASSED=0
    DIR_SEGFAULTS=0

    # Run tests for each .in file in the directory
    for in_file in "$dir"/*.in; do
        # Skip if no .in files exist
        [ -e "$in_file" ] || continue

        ((DIR_TOTAL++))
        test_name=$(basename "$in_file" .in)
        echo "Running test: $test_name"

        # Run hw3 and save output
        ./hw3 < "$in_file" > "$dir/$test_name.res" 2>&1
        EXIT_CODE=$?

        # Check for segmentation fault (exit code 139)
        if [ $EXIT_CODE -eq 139 ]; then
            SEGFAULT_TESTS+=("$test_name")
            ALL_SEGFAULT_TESTS+=("$dir_name/$test_name")
            FAILED_TESTS+=("$test_name")
            ALL_FAILED_TESTS+=("$dir_name/$test_name")
            ((TOTAL_FAILED++))
            ((DIR_SEGFAULTS++))
            ((TOTAL_SEGFAULTS++))
            echo "  ⚠️ SEGMENTATION FAULT detected!"
            continue
        fi

        # Compare with expected output
        diff -q "$dir/$test_name.res" "$dir/$test_name.out" > /dev/null
        if [ $? -ne 0 ]; then
            FAILED_TESTS+=("$test_name")
            ALL_FAILED_TESTS+=("$dir_name/$test_name")
            ((TOTAL_FAILED++))
        else
            ((DIR_PASSED++))
            ((TOTAL_PASSED++))
        fi
    done

    # Store results for this directory
    DIR_FAILED=$((DIR_TOTAL - DIR_PASSED))
    FOLDER_RESULTS+=("$dir_name:$DIR_PASSED:$DIR_FAILED:$DIR_TOTAL:$DIR_SEGFAULTS")

    # Report results for this directory
    if [ ${#FAILED_TESTS[@]} -eq 0 ]; then
        echo "All tests in $dir_name passed ✅"
    else
        echo "Failed tests in $dir_name ❌:"
        for test in "${FAILED_TESTS[@]}"; do
            echo "  $test"
        done
        
        if [ ${#SEGFAULT_TESTS[@]} -gt 0 ]; then
            echo "Tests with segmentation faults in $dir_name ⚠️:"
            for test in "${SEGFAULT_TESTS[@]}"; do
                echo "  $test"
            done
        fi
    fi
    echo ""
done

# Report overall results
echo "==============================================="
echo "SUMMARY"
echo "==============================================="

TOTAL_TESTS=$((TOTAL_PASSED + TOTAL_FAILED))

# Print folder-by-folder summary table
echo "Results by folder:"
echo "-----------------"
printf "%-20s %-10s %-10s %-10s %-12s %s\n" "Folder" "Passed" "Failed" "Total" "Segfaults" "Status"
printf "%-20s %-10s %-10s %-10s %-12s %s\n" "------" "------" "------" "-----" "---------" "------"

for result in "${FOLDER_RESULTS[@]}"; do
    IFS=':' read -r folder passed failed total segfaults <<< "$result"
    if [ "$failed" -eq 0 ]; then
        status="✅ PASSED"
    else
        status="❌ FAILED"
    fi
    printf "%-20s %-10s %-10s %-10s %-12s %s\n" "$folder" "$passed" "$failed" "$total" "$segfaults" "$status"
done

echo ""
echo "Overall: $TOTAL_PASSED passed, $TOTAL_FAILED failed out of $TOTAL_TESTS tests"
echo "Total segmentation faults: $TOTAL_SEGFAULTS"

if [ $TOTAL_FAILED -eq 0 ]; then
    echo "All tests in all directories passed ✅"
else
    echo "$TOTAL_FAILED test(s) failed across all directories ❌"
    echo "Failed tests:"
    for test in "${ALL_FAILED_TESTS[@]}"; do
        echo "  $test"
    done
    
    if [ $TOTAL_SEGFAULTS -gt 0 ]; then
        echo ""
        echo "⚠️ Tests with segmentation faults:"
        for test in "${ALL_SEGFAULT_TESTS[@]}"; do
            echo "  $test"
        done
    fi
fi