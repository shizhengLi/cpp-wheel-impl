cmake_minimum_required(VERSION 3.10)
project(stl_impl)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# 查找必要的包
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# 设置包含目录
include_directories(include)

# 添加子目录
add_subdirectory(test)

# 启用测试
enable_testing()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 编译定义
add_definitions(-DSTL_IMPL_DEBUG)

# 性能优化选项
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DNDEBUG -O3)
else()
    add_definitions(-DDEBUG -g)
endif()