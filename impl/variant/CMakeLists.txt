cmake_minimum_required(VERSION 3.10)
project(variant_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译警告
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# 查找Google Test
find_package(GTest REQUIRED)

# 添加测试可执行文件
add_executable(variant_test test/variant_test.cpp)

# 链接Google Test
target_link_libraries(variant_test GTest::GTest GTest::Main)

# 设置输出目录
set_target_properties(variant_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 启用测试
enable_testing()
add_test(NAME variant_test COMMAND variant_test)