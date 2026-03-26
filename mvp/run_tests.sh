#!/bin/bash

# Flower Exchange Test Runner
# Runs all tests from tests/input and saves results to tests/output

echo "╔═══════════════════════════════════════════════════╗"
echo "║    Flower Exchange - Running All Test Cases      ║"
echo "╚═══════════════════════════════════════════════════╝"
echo ""

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# Executable path
EXECUTABLE="./flower-exchange.exe"

# Test directories
INPUT_DIR="tests/input"
OUTPUT_DIR="tests/output"

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

# Counter for tracking
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

echo "Input directory:  $INPUT_DIR"
echo "Output directory: $OUTPUT_DIR"
echo ""
echo "═══════════════════════════════════════════════════"
echo "  Processing all test cases"
echo "═══════════════════════════════════════════════════"
echo ""

# Run all tests from tests/input folder
if [ -d "$INPUT_DIR" ]; then
    for input_file in "$INPUT_DIR"/*.csv; do
        if [ -f "$input_file" ]; then
            test_name=$(basename "$input_file" .csv)
            output_file="$OUTPUT_DIR/${test_name}_output.csv"

            echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
            echo "Running: $test_name"
            echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

            if "$EXECUTABLE" "$input_file" "$output_file" > /dev/null 2>&1; then
                echo "✓ SUCCESS: Output saved to $output_file"
                ((PASSED_TESTS++))
            else
                echo "✗ FAILED: Error processing $input_file"
                ((FAILED_TESTS++))
            fi

            ((TOTAL_TESTS++))
            echo ""
        fi
    done
else
    echo "⚠ Error: $INPUT_DIR directory not found"
    echo "Please make sure test CSV files are in $INPUT_DIR/"
    exit 1
fi

# Print summary
echo "╔═══════════════════════════════════════════════════╗"
echo "║              TEST EXECUTION SUMMARY               ║"
echo "╚═══════════════════════════════════════════════════╝"
echo ""
echo "Total tests run:     $TOTAL_TESTS"
echo "Passed:              $PASSED_TESTS ✓"
echo "Failed:              $FAILED_TESTS ✗"
echo ""
echo "All output files saved to: $OUTPUT_DIR/"
echo ""

if [ $FAILED_TESTS -eq 0 ]; then
    echo "🎉 All tests completed successfully!"
else
    echo "⚠ Some tests failed. Please review the output."
fi

echo ""
