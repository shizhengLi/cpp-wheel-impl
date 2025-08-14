#!/bin/bash

# Comprehensive test script for STL-impl Test Suite
# This script provides a simple way to run all tests with one command

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
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

print_header() {
    echo -e "${CYAN}===========================================${NC}"
    echo -e "${CYAN}$1${NC}"
    echo -e "${CYAN}===========================================${NC}"
}

# Check if we're in the correct directory
if [ ! -f "Makefile.tests" ]; then
    print_error "Please run this script from the test/unit directory"
    exit 1
fi

# Function to show help
show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -a, --all           Run all tests (default)"
    echo "  -r, --regular       Run only regular STL tests"
    echo "  -s, --hash          Run only hash container tests"
    echo "  -q, --quick         Run quick test (first available test)"
    echo "  -c, --clean         Clean build artifacts"
    echo "  -b, --rebuild       Clean and rebuild all tests"
    echo "  --stats             Show test statistics"
    echo "  --check-gtest       Check Google Test installation"
    echo "  --install-gtest     Install Google Test"
    echo ""
    echo "Examples:"
    echo "  $0                  # Run all tests"
    echo "  $0 -r               # Run only regular tests"
    echo "  $0 -s               # Run only hash container tests"
    echo "  $0 -c               # Clean build artifacts"
    echo "  $0 --stats          # Show test statistics"
}

# Function to run regular tests
run_regular_tests() {
    print_header "Running Regular STL Tests"
    
    # Build all tests first
    print_status "Building all regular tests..."
    if ! make -f Makefile.tests all >/dev/null 2>&1; then
        print_error "Failed to build regular tests"
        return 1
    fi
    
    # Run tests
    print_status "Running regular tests..."
    if make -f Makefile.tests test_all; then
        print_success "All regular tests passed!"
        return 0
    else
        print_error "Some regular tests failed"
        return 1
    fi
}

# Function to run hash container tests
run_hash_tests() {
    print_header "Running Hash Container Tests"
    
    if make -f test_hash_containers.mk test_all; then
        print_success "All hash container tests passed!"
        return 0
    else
        print_error "Some hash container tests failed"
        return 1
    fi
}

# Function to run quick test
run_quick_test() {
    print_header "Running Quick Test"
    
    if make -f Makefile.tests quick_test; then
        print_success "Quick test passed!"
        return 0
    else
        print_error "Quick test failed"
        return 1
    fi
}

# Function to show statistics
show_stats() {
    print_header "Test Suite Statistics"
    make -f Makefile.tests stats
}

# Function to clean build artifacts
clean_build() {
    print_status "Cleaning build artifacts..."
    make -f Makefile.tests clean
    print_success "Clean completed"
}

# Function to rebuild all tests
rebuild_tests() {
    print_status "Rebuilding all tests..."
    make -f Makefile.tests rebuild
    print_success "Rebuild completed"
}

# Function to check Google Test installation
check_gtest() {
    print_status "Checking Google Test installation..."
    make -f Makefile.tests check_gtest
}

# Function to install Google Test
install_gtest() {
    print_status "Installing Google Test..."
    make -f Makefile.tests install_gtest
}

# Main function
main() {
    local mode="all"
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -a|--all)
                mode="all"
                shift
                ;;
            -r|--regular)
                mode="regular"
                shift
                ;;
            -s|--hash)
                mode="hash"
                shift
                ;;
            -q|--quick)
                mode="quick"
                shift
                ;;
            -c|--clean)
                clean_build
                exit 0
                ;;
            -b|--rebuild)
                rebuild_tests
                exit 0
                ;;
            --stats)
                show_stats
                exit 0
                ;;
            --check-gtest)
                check_gtest
                exit 0
                ;;
            --install-gtest)
                install_gtest
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # Execute based on mode
    case $mode in
        "all")
            print_header "STL-impl Test Suite - Full Run"
            if run_regular_tests && run_hash_tests; then
                print_success "All tests completed successfully!"
                exit 0
            else
                print_error "Some tests failed"
                exit 1
            fi
            ;;
        "regular")
            run_regular_tests
            ;;
        "hash")
            run_hash_tests
            ;;
        "quick")
            run_quick_test
            ;;
    esac
}

# Check if no arguments provided (default behavior)
if [ $# -eq 0 ]; then
    main
else
    # Parse arguments
    main "$@"
fi