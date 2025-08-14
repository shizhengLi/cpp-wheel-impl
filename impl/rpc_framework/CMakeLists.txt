cmake_minimum_required(VERSION 3.10)
project(rpc_framework_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pthread")

# Find required packages
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Add test executable
add_executable(rpc_framework_test test/rpc_framework_simple_test.cpp include/rpc_client.cpp include/rpc_server.cpp include/rpc_serializer.cpp include/rpc_protocol.cpp)

# Link libraries
target_link_libraries(rpc_framework_test GTest::GTest GTest::Main Threads::Threads)

# Include directories
target_include_directories(rpc_framework_test PRIVATE include)

# Set output directory
set_target_properties(rpc_framework_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Enable testing
enable_testing()

# Add test
add_test(NAME rpc_framework_test COMMAND rpc_framework_test)