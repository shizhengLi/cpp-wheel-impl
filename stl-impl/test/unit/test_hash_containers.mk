# Makefile for testing hash-based containers
# This makefile provides simple commands to run all tests

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
INCLUDES = -I../../include

# Google Test settings
GTEST_LIBS = -lgtest -lgtest_main -pthread

# Source files
HASH_TABLE_TEST_SRC = hash_table_basic_test.cpp
UNORDERED_SET_TEST_SRC = unordered_set_basic_test.cpp
UNORDERED_MAP_TEST_SRC = unordered_map_basic_test.cpp

# Executable names
HASH_TABLE_TEST = hash_table_basic_test
UNORDERED_SET_TEST = unordered_set_basic_test
UNORDERED_MAP_TEST = unordered_map_basic_test

# Default target
all: $(HASH_TABLE_TEST) $(UNORDERED_SET_TEST) $(UNORDERED_MAP_TEST)

# Build hash_table_test
$(HASH_TABLE_TEST): $(HASH_TABLE_TEST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ $(GTEST_LIBS)

# Build unordered_set_test
$(UNORDERED_SET_TEST): $(UNORDERED_SET_TEST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ $(GTEST_LIBS)

# Build unordered_map_test
$(UNORDERED_MAP_TEST): $(UNORDERED_MAP_TEST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ $(GTEST_LIBS)

# Run individual tests
test_hash_table: $(HASH_TABLE_TEST)
	./$(HASH_TABLE_TEST)

test_unordered_set: $(UNORDERED_SET_TEST)
	./$(UNORDERED_SET_TEST)

test_unordered_map: $(UNORDERED_MAP_TEST)
	./$(UNORDERED_MAP_TEST)

# Run all tests
test_all: $(HASH_TABLE_TEST) $(UNORDERED_SET_TEST) $(UNORDERED_MAP_TEST)
	@echo "Running hash_table_test..."
	./$(HASH_TABLE_TEST)
	@echo "Running unordered_set_test..."
	./$(UNORDERED_SET_TEST)
	@echo "Running unordered_map_test..."
	./$(UNORDERED_MAP_TEST)
	@echo "All tests completed!"

# Clean build artifacts
clean:
	rm -f $(HASH_TABLE_TEST) $(UNORDERED_SET_TEST) $(UNORDERED_MAP_TEST)

# Rebuild all tests
rebuild: clean all

# Install Google Test (if needed)
install_gtest:
	@echo "Installing Google Test..."
	sudo apt-get update
	sudo apt-get install -y libgtest-dev libgtest1

# Check if Google Test is installed
check_gtest:
	@echo "Checking Google Test installation..."
	pkg-config --exists gtest && echo "Google Test is installed" || echo "Google Test is not installed"

# Help message
help:
	@echo "Available targets:"
	@echo "  all              - Build all test executables"
	@echo "  test_hash_table  - Build and run hash_table_test"
	@echo "  test_unordered_set - Build and run unordered_set_test"
	@echo "  test_unordered_map - Build and run unordered_map_test"
	@echo "  test_all         - Build and run all tests"
	@echo "  clean            - Remove all built files"
	@echo "  rebuild          - Clean and rebuild all tests"
	@echo "  install_gtest    - Install Google Test"
	@echo "  check_gtest      - Check if Google Test is installed"
	@echo "  help             - Show this help message"

# Phony targets
.PHONY: all test_hash_table test_unordered_set test_unordered_map test_all clean rebuild install_gtest check_gtest help