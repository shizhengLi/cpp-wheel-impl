cmake_minimum_required(VERSION 3.10)
project(thread_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译警告
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# 查找Google Test
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# 添加测试可执行文件
add_executable(thread_test test/thread_test.cpp)
add_executable(minimal_test test/minimal_test.cpp)

# 链接Google Test和pthreads
target_link_libraries(thread_test GTest::GTest GTest::Main Threads::Threads)
target_link_libraries(minimal_test GTest::GTest GTest::Main Threads::Threads)

# 设置输出目录
set_target_properties(thread_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(minimal_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 启用测试
enable_testing()
add_test(NAME thread_test COMMAND thread_test)