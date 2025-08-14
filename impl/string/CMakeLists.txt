cmake_minimum_required(VERSION 3.10)
project(string_impl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译器警告
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# 查找Google Test
find_package(GTest REQUIRED)

# 添加测试可执行文件
add_executable(string_test
    test/string_test.cpp
)

# 链接Google Test
target_link_libraries(string_test PRIVATE GTest::gtest GTest::gtest_main)

# 包含头文件目录
target_include_directories(string_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 设置输出目录
set_target_properties(string_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 如果启用测试
enable_testing()
add_test(NAME string_test COMMAND string_test)