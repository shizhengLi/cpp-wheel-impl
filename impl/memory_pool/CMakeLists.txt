cmake_minimum_required(VERSION 3.10)
project(memory_pool_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译器警告
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# 查找GTest
find_package(GTest REQUIRED)

# 添加测试可执行文件
add_executable(memory_pool_test test/memory_pool_test.cpp)

# 链接GTest
target_link_libraries(memory_pool_test GTest::GTest GTest::Main)

# 包含头文件目录
target_include_directories(memory_pool_test PRIVATE include)

# 设置输出目录
set_target_properties(memory_pool_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# 启用测试
enable_testing()

# 添加测试
add_test(NAME memory_pool_test COMMAND memory_pool_test)