cmake_minimum_required(VERSION 3.10)
project(epoll_event_loop_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pthread")

# Find required packages
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Add test executable
add_executable(epoll_event_loop_test test/epoll_event_loop_test.cpp include/epoll_event_loop.cpp)

# Link libraries
target_link_libraries(epoll_event_loop_test GTest::GTest GTest::Main Threads::Threads)

# Include directories
target_include_directories(epoll_event_loop_test PRIVATE include)

# Set output directory
set_target_properties(epoll_event_loop_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Enable testing
enable_testing()

# Add test
add_test(NAME epoll_event_loop_test COMMAND epoll_event_loop_test)