cmake_minimum_required(VERSION 3.10)
project(thread_pool_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译器警告
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pthread")

# 查找GTest
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# 添加测试可执行文件
add_executable(thread_pool_test test/thread_pool_test.cpp)

# 链接GTest和线程库
target_link_libraries(thread_pool_test GTest::GTest GTest::Main Threads::Threads)

# 包含头文件目录
target_include_directories(thread_pool_test PRIVATE include)

# 设置输出目录
set_target_properties(thread_pool_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# 启用测试
enable_testing()

# 添加测试
add_test(NAME thread_pool_test COMMAND thread_pool_test)