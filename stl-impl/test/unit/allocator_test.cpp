#include <gtest/gtest.h>
#include <cstddef>
#include <memory>

// 先包含我们即将实现的allocator
#include "stl/allocator.hpp"

namespace stl {

// 测试基本内存分配和释放
TEST(AllocatorTest, BasicAllocateDeallocate) {
    allocator<int> alloc;
    
    // 分配一个int的内存
    int* p = alloc.allocate(1);
    EXPECT_NE(p, nullptr);
    
    // 释放内存
    alloc.deallocate(p, 1);
}

// 测试多个对象的内存分配
TEST(AllocatorTest, MultipleAllocateDeallocate) {
    allocator<int> alloc;
    const size_t n = 10;
    
    // 分配10个int的内存
    int* p = alloc.allocate(n);
    EXPECT_NE(p, nullptr);
    
    // 释放内存
    alloc.deallocate(p, n);
}

// 测试对象构造和销毁
TEST(AllocatorTest, ConstructDestroy) {
    allocator<int> alloc;
    
    // 分配内存
    int* p = alloc.allocate(1);
    
    // 构造对象
    alloc.construct(p, 42);
    EXPECT_EQ(*p, 42);
    
    // 销毁对象
    alloc.destroy(p);
    
    // 释放内存
    alloc.deallocate(p, 1);
}

// 测试多个对象的构造和销毁
TEST(AllocatorTest, MultipleConstructDestroy) {
    allocator<int> alloc;
    const size_t n = 5;
    
    // 分配内存
    int* p = alloc.allocate(n);
    
    // 构造多个对象
    for (size_t i = 0; i < n; ++i) {
        alloc.construct(p + i, static_cast<int>(i * 10));
        EXPECT_EQ(p[i], static_cast<int>(i * 10));
    }
    
    // 销毁多个对象
    for (size_t i = 0; i < n; ++i) {
        alloc.destroy(p + i);
    }
    
    // 释放内存
    alloc.deallocate(p, n);
}

// 测试rebind功能
TEST(AllocatorTest, Rebind) {
    allocator<int> int_alloc;
    
    // 使用rebind创建double的分配器
    using double_allocator = allocator<int>::rebind<double>::other;
    double_allocator double_alloc = int_alloc;
    
    // 分配double
    double* p = double_alloc.allocate(1);
    EXPECT_NE(p, nullptr);
    
    // 构造double
    double_alloc.construct(p, 3.14);
    EXPECT_DOUBLE_EQ(*p, 3.14);
    
    // 销毁和释放
    double_alloc.destroy(p);
    double_alloc.deallocate(p, 1);
}

// 测试不同类型的分配器
TEST(AllocatorTest, DifferentTypes) {
    // 测试基本类型
    allocator<int> int_alloc;
    int* int_p = int_alloc.allocate(1);
    int_alloc.construct(int_p, 100);
    EXPECT_EQ(*int_p, 100);
    int_alloc.destroy(int_p);
    int_alloc.deallocate(int_p, 1);
    
    // 测试自定义类型
    struct TestStruct {
        int x;
        double y;
        TestStruct(int a, double b) : x(a), y(b) {}
    };
    
    allocator<TestStruct> struct_alloc;
    TestStruct* struct_p = struct_alloc.allocate(1);
    struct_alloc.construct(struct_p, 42, 2.718);
    EXPECT_EQ(struct_p->x, 42);
    EXPECT_DOUBLE_EQ(struct_p->y, 2.718);
    struct_alloc.destroy(struct_p);
    struct_alloc.deallocate(struct_p, 1);
}

// 测试最大大小
TEST(AllocatorTest, MaxSize) {
    allocator<int> alloc;
    allocator<double> double_alloc;
    
    size_t int_max = alloc.max_size();
    size_t double_max = double_alloc.max_size();
    
    EXPECT_GT(int_max, 0u);
    EXPECT_GT(double_max, 0u);
    EXPECT_GE(int_max, double_max); // int通常比double小
}

// 测试异常安全性
TEST(AllocatorTest, ExceptionSafety) {
    allocator<std::string> alloc;
    
    std::string* p = alloc.allocate(1);
    EXPECT_NE(p, nullptr);
    
    // 构造可能抛异常的对象
    try {
        alloc.construct(p, "Hello World");
        EXPECT_EQ(*p, "Hello World");
        alloc.destroy(p);
    } catch (...) {
        // 即使构造抛异常，也应该能正确释放内存
        alloc.deallocate(p, 1);
        throw;
    }
    
    alloc.deallocate(p, 1);
}

// 测试大块内存分配
TEST(AllocatorTest, LargeAllocation) {
    allocator<int> alloc;
    const size_t large_size = 10000;
    
    int* p = alloc.allocate(large_size);
    EXPECT_NE(p, nullptr);
    
    // 写入和读取验证
    for (size_t i = 0; i < large_size; ++i) {
        alloc.construct(p + i, static_cast<int>(i));
        EXPECT_EQ(p[i], static_cast<int>(i));
    }
    
    // 清理
    for (size_t i = 0; i < large_size; ++i) {
        alloc.destroy(p + i);
    }
    
    alloc.deallocate(p, large_size);
}

// 测试零大小分配
TEST(AllocatorTest, ZeroSizeAllocation) {
    allocator<int> alloc;
    
    // 零大小分配应该返回非空指针
    int* p = alloc.allocate(0);
    // 标准允许返回nullptr或非空指针
    alloc.deallocate(p, 0);
}

// 测试地址函数
TEST(AllocatorTest, AddressFunctions) {
    allocator<int> alloc;
    int value = 42;
    
    // 测试常量版本
    const int& const_value = value;
    const int* const_addr = alloc.address(const_value);
    EXPECT_EQ(const_addr, &value);
    
    // 测试非常量版本
    int* addr = alloc.address(value);
    EXPECT_EQ(addr, &value);
}

} // namespace stl