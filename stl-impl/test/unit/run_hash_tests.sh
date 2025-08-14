#!/bin/bash

# Test script for hash-based containers
# This script provides a simple way to run all tests with one command

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're in the correct directory
if [ ! -f "test_hash_containers.mk" ]; then
    print_error "Please run this script from the test/unit directory"
    exit 1
fi

# Function to run a single test
run_test() {
    local test_name=$1
    local test_file="${test_name}.cpp"
    local test_exec="${test_name}"
    
    print_status "Building $test_name..."
    
    # Check if source file exists
    if [ ! -f "$test_file" ]; then
        print_error "Test file $test_file not found"
        return 1
    fi
    
    # Build the test
    if ! make -f test_hash_containers.mk "$test_exec"; then
        print_error "Failed to build $test_name"
        return 1
    fi
    
    # Run the test
    print_status "Running $test_name..."
    if ./"$test_exec"; then
        print_success "$test_name passed"
        return 0
    else
        print_error "$test_name failed"
        return 1
    fi
}

# Main test execution
main() {
    print_status "Starting hash-based containers tests..."
    
    # List of tests to run
    tests=("hash_table_basic_test" "unordered_set_basic_test" "unordered_map_basic_test")
    
    failed_tests=()
    
    # Run each test
    for test in "${tests[@]}"; do
        if ! run_test "$test"; then
            failed_tests+=("$test")
        fi
        echo  # Add blank line between tests
    done
    
    # Summary
    if [ ${#failed_tests[@]} -eq 0 ]; then
        print_success "All tests passed!"
        exit 0
    else
        print_error "The following tests failed:"
        for failed_test in "${failed_tests[@]}"; do
            echo "  - $failed_test"
        done
        exit 1
    fi
}

# Check for help flag
if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help     Show this help message"
    echo "  clean          Clean build artifacts"
    echo "  rebuild        Clean and rebuild all tests"
    echo ""
    echo "Examples:"
    echo "  $0              # Run all tests"
    echo "  $0 clean        # Clean build artifacts"
    echo "  $0 rebuild      # Clean and rebuild all tests"
    exit 0
fi

# Handle other commands
case "$1" in
    "clean")
        print_status "Cleaning build artifacts..."
        make -f test_hash_containers.mk clean
        print_success "Clean completed"
        ;;
    "rebuild")
        print_status "Rebuilding all tests..."
        make -f test_hash_containers.mk rebuild
        print_success "Rebuild completed"
        ;;
    *)
        main
        ;;
esac