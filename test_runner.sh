#!/bin/bash

# ec_visualizer Test Runner
set -e

echo "=========================================="
echo "  ec_visualizer v2.2.0 - Test Suite"
echo "=========================================="

BUILD_DIR="${BUILD_DIR:-build}"

if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Build directory not found. Run cmake first."
    exit 1
fi

cd "$BUILD_DIR"

echo ""
echo "Running CTest..."
ctest --output-on-failure --verbose

echo ""
echo "=========================================="
echo "  All tests completed!"
echo "=========================================="
