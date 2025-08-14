#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include "stl/vector.hpp"

using namespace stl;

class VectorTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// 构造函数测试
TEST_F(VectorTest, Constructors) {
    // 默认构造函数
    vector<int> v1;
    EXPECT_TRUE(v1.empty());
    EXPECT_EQ(v1.size(), 0);
    EXPECT_EQ(v1.capacity(), 0);
    
    // 指定大小构造函数
    vector<int> v2(5);
    EXPECT_EQ(v2.size(), 5);
    EXPECT_GE(v2.capacity(), 5);
    
    // 指定大小和值构造函数
    vector<int> v3(3, 42);
    EXPECT_EQ(v3.size(), 3);
    EXPECT_EQ(v3[0], 42);
    EXPECT_EQ(v3[1], 42);
    EXPECT_EQ(v3[2], 42);
    
    // 迭代器构造函数
    std::vector<int> std_vec = {1, 2, 3, 4, 5};
    vector<int> v4(std_vec.begin(), std_vec.end());
    EXPECT_EQ(v4.size(), 5);
    EXPECT_EQ(v4[0], 1);
    EXPECT_EQ(v4[4], 5);
    
    // 拷贝构造函数
    vector<int> v5(v4);
    EXPECT_EQ(v5.size(), v4.size());
    EXPECT_EQ(v5[0], v4[0]);
    EXPECT_EQ(v5[4], v4[4]);
    
    // 移动构造函数
    vector<int> v6(std::move(v5));
    EXPECT_EQ(v6.size(), 5);
    EXPECT_EQ(v6[0], 1);
    EXPECT_TRUE(v5.empty());
    
    // 初始化列表构造函数
    vector<int> v7 = {1, 2, 3, 4, 5};
    EXPECT_EQ(v7.size(), 5);
    EXPECT_EQ(v7[0], 1);
    EXPECT_EQ(v7[4], 5);
}

// 赋值运算符测试
TEST_F(VectorTest, AssignmentOperators) {
    vector<int> v1 = {1, 2, 3};
    vector<int> v2;
    
    // 拷贝赋值
    v2 = v1;
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2[0], 1);
    EXPECT_EQ(v2[2], 3);
    
    // 移动赋值
    vector<int> v3;
    v3 = std::move(v2);
    EXPECT_EQ(v3.size(), 3);
    EXPECT_EQ(v3[0], 1);
    EXPECT_TRUE(v2.empty());
    
    // 初始化列表赋值
    vector<int> v4;
    v4 = {4, 5, 6, 7};
    EXPECT_EQ(v4.size(), 4);
    EXPECT_EQ(v4[0], 4);
    EXPECT_EQ(v4[3], 7);
}

// 元素访问测试
TEST_F(VectorTest, ElementAccess) {
    vector<int> v = {1, 2, 3, 4, 5};
    
    // operator[]
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[4], 5);
    v[0] = 10;
    EXPECT_EQ(v[0], 10);
    
    // at()
    EXPECT_EQ(v.at(1), 2);
    EXPECT_EQ(v.at(4), 5);
    v.at(2) = 20;
    EXPECT_EQ(v.at(2), 20);
    
    // 异常测试
    EXPECT_THROW(v.at(5), vector_exception);
    EXPECT_THROW(v.at(10), vector_exception);
    
    // front() 和 back()
    EXPECT_EQ(v.front(), 10);
    EXPECT_EQ(v.back(), 5);
    
    // data()
    EXPECT_EQ(*v.data(), 10);
    EXPECT_EQ(v.data()[4], 5);
}

// 迭代器测试
TEST_F(VectorTest, Iterators) {
    vector<int> v = {1, 2, 3, 4, 5};
    
    // 正向迭代器
    auto it = v.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    --it;
    EXPECT_EQ(*it, 1);
    
    // 常量迭代器
    auto cit = v.cbegin();
    EXPECT_EQ(*cit, 1);
    ++cit;
    EXPECT_EQ(*cit, 2);
    
    // 反向迭代器
    auto rit = v.rbegin();
    EXPECT_EQ(*rit, 5);
    ++rit;
    EXPECT_EQ(*rit, 4);
    
    // 算法测试
    EXPECT_EQ(std::find(v.begin(), v.end(), 3), v.begin() + 2);
    EXPECT_EQ(std::count(v.begin(), v.end(), 2), 1);
}

// 容量测试
TEST_F(VectorTest, Capacity) {
    vector<int> v;
    
    // 空测试
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
    
    // 添加元素
    v.push_back(1);
    EXPECT_FALSE(v.empty());
    EXPECT_EQ(v.size(), 1);
    EXPECT_GE(v.capacity(), 1);
    
    // 预留容量
    v.reserve(10);
    EXPECT_EQ(v.size(), 1);
    EXPECT_GE(v.capacity(), 10);
    
    // 收缩到适合大小
    v.push_back(2);
    v.push_back(3);
    v.shrink_to_fit();
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v.capacity(), 3);
}

// 修改器测试
TEST_F(VectorTest, Modifiers) {
    vector<int> v = {1, 2, 3, 4, 5};
    
    // clear
    v.clear();
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
    
    // push_back
    v.push_back(10);
    v.push_back(20);
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
    
    // pop_back
    v.pop_back();
    EXPECT_EQ(v.size(), 1);
    EXPECT_EQ(v[0], 10);
    
    // resize
    v.resize(5, 42);
    EXPECT_EQ(v.size(), 5);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 42);
    EXPECT_EQ(v[4], 42);
    
    // resize缩小
    v.resize(2);
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 42);
}

// insert测试
TEST_F(VectorTest, Insert) {
    vector<int> v = {1, 2, 3, 4, 5};
    
    // 在开头插入
    auto it = v.insert(v.begin(), 0);
    EXPECT_EQ(v.size(), 6);
    EXPECT_EQ(v[0], 0);
    EXPECT_EQ(v[1], 1);
    EXPECT_EQ(it, v.begin());
    
    // 在中间插入
    it = v.insert(v.begin() + 3, 99);
    EXPECT_EQ(v.size(), 7);
    EXPECT_EQ(v[3], 99);
    EXPECT_EQ(v[4], 3);
    
    // 在末尾插入
    it = v.insert(v.end(), 100);
    EXPECT_EQ(v.size(), 8);
    EXPECT_EQ(v[7], 100);
    
    // 插入多个元素
    v.insert(v.begin() + 2, 3, 77);
    EXPECT_EQ(v.size(), 11);
    EXPECT_EQ(v[2], 77);
    EXPECT_EQ(v[3], 77);
    EXPECT_EQ(v[4], 77);
    
    // 迭代器范围插入
    std::vector<int> src = {20, 21, 22};
    v.insert(v.begin() + 1, src.begin(), src.end());
    EXPECT_EQ(v.size(), 14);
    EXPECT_EQ(v[1], 20);
    EXPECT_EQ(v[2], 21);
    EXPECT_EQ(v[3], 22);
    
    // 初始化列表插入
    v.insert(v.begin() + 5, {30, 31, 32});
    EXPECT_EQ(v.size(), 17);
    EXPECT_EQ(v[5], 30);
    EXPECT_EQ(v[6], 31);
    EXPECT_EQ(v[7], 32);
}

// emplace测试
TEST_F(VectorTest, Emplace) {
    vector<std::pair<int, std::string>> v;
    
    // emplace_back
    v.emplace_back(1, "hello");
    v.emplace_back(2, "world");
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[0].first, 1);
    EXPECT_EQ(v[0].second, "hello");
    
    // emplace
    auto it = v.emplace(v.begin() + 1, 3, "test");
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[1].first, 3);
    EXPECT_EQ(v[1].second, "test");
    EXPECT_EQ(it, v.begin() + 1);
}

// erase测试
TEST_F(VectorTest, Erase) {
    vector<int> v = {1, 2, 3, 4, 5};
    
    // 删除单个元素
    auto it = v.erase(v.begin() + 2);
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 4);
    EXPECT_EQ(v[3], 5);
    EXPECT_EQ(it, v.begin() + 2);
    
    // 删除范围
    it = v.erase(v.begin() + 1, v.begin() + 3);
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 5);
    EXPECT_EQ(it, v.begin() + 1);
}

// assign测试
TEST_F(VectorTest, Assign) {
    vector<int> v = {1, 2, 3};
    
    // 值赋值
    v.assign(5, 10);
    EXPECT_EQ(v.size(), 5);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[4], 10);
    
    // 迭代器范围赋值
    std::vector<int> src = {20, 21, 22, 23, 24};
    v.assign(src.begin(), src.end());
    EXPECT_EQ(v.size(), 5);
    EXPECT_EQ(v[0], 20);
    EXPECT_EQ(v[4], 24);
    
    // 初始化列表赋值
    v.assign({30, 31, 32});
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 30);
    EXPECT_EQ(v[2], 32);
}

// swap测试
TEST_F(VectorTest, Swap) {
    vector<int> v1 = {1, 2, 3};
    vector<int> v2 = {4, 5, 6, 7};
    
    v1.swap(v2);
    
    EXPECT_EQ(v1.size(), 4);
    EXPECT_EQ(v1[0], 4);
    EXPECT_EQ(v1[3], 7);
    
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2[0], 1);
    EXPECT_EQ(v2[2], 3);
}

// 比较运算符测试
TEST_F(VectorTest, ComparisonOperators) {
    vector<int> v1 = {1, 2, 3};
    vector<int> v2 = {1, 2, 3};
    vector<int> v3 = {1, 2, 4};
    vector<int> v4 = {1, 2};
    vector<int> v5 = {1, 2, 3, 4};
    
    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 == v3);
    EXPECT_FALSE(v1 == v4);
    EXPECT_FALSE(v1 == v5);
    
    EXPECT_TRUE(v1 != v3);
    EXPECT_FALSE(v1 != v2);
    
    EXPECT_TRUE(v1 < v3);
    EXPECT_FALSE(v1 < v2);
    EXPECT_FALSE(v3 < v1);
    
    EXPECT_TRUE(v1 <= v2);
    EXPECT_TRUE(v1 <= v3);
    EXPECT_FALSE(v3 <= v1);
    
    EXPECT_TRUE(v3 > v1);
    EXPECT_FALSE(v1 > v2);
    
    EXPECT_TRUE(v1 >= v2);
    EXPECT_TRUE(v3 >= v1);
    EXPECT_FALSE(v1 >= v3);
}

// 移动语义测试
TEST_F(VectorTest, MoveSemantics) {
    vector<std::unique_ptr<int>> v;
    
    // push_back移动语义
    v.push_back(std::make_unique<int>(42));
    EXPECT_EQ(*v[0], 42);
    
    // emplace_back移动语义
    auto ptr = std::make_unique<int>(100);
    v.emplace_back(std::move(ptr));
    EXPECT_EQ(*v[1], 100);
    EXPECT_EQ(ptr.get(), nullptr);
    
    // insert移动语义
    auto ptr2 = std::make_unique<int>(200);
    v.insert(v.begin(), std::move(ptr2));
    EXPECT_EQ(*v[0], 200);
    EXPECT_EQ(ptr2.get(), nullptr);
}

// 异常安全测试
TEST_F(VectorTest, ExceptionSafety) {
    vector<int> v = {1, 2, 3};
    
    // at()异常
    EXPECT_THROW(v.at(3), vector_exception);
    EXPECT_THROW(v.at(10), vector_exception);
    
    // empty()不会抛出异常
    EXPECT_NO_THROW(v.empty());
    
    // size()不会抛出异常
    EXPECT_NO_THROW(v.size());
    
    // capacity()不会抛出异常
    EXPECT_NO_THROW(v.capacity());
}

// 内存管理测试
TEST_F(VectorTest, MemoryManagement) {
    vector<int> v;
    
    // 初始状态
    EXPECT_EQ(v.data(), nullptr);
    EXPECT_EQ(v.capacity(), 0);
    
    // 添加元素后
    v.push_back(1);
    EXPECT_NE(v.data(), nullptr);
    EXPECT_GE(v.capacity(), 1);
    
    // 保存数据指针
    int* old_data = v.data();
    size_t old_capacity = v.capacity();
    
    // 触发重新分配
    for (int i = 0; i < 100; ++i) {
        v.push_back(i);
    }
    
    // 验证重新分配
    EXPECT_NE(v.data(), old_data);
    EXPECT_GT(v.capacity(), old_capacity);
    
    // shrink_to_fit
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), v.size());
}

// 标准库兼容性测试
TEST_F(VectorTest, StandardLibraryCompatibility) {
    vector<int> v = {1, 2, 3, 4, 5};
    
    // 与STL算法配合使用
    std::sort(v.begin(), v.end());
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[4], 5);
    
    std::reverse(v.begin(), v.end());
    EXPECT_EQ(v[0], 5);
    EXPECT_EQ(v[4], 1);
    
    // std::find
    auto it = std::find(v.begin(), v.end(), 3);
    EXPECT_NE(it, v.end());
    EXPECT_EQ(*it, 3);
    
    // std::count
    EXPECT_EQ(std::count(v.begin(), v.end(), 3), 1);
    
    // std::for_each
    int sum = 0;
    std::for_each(v.begin(), v.end(), [&sum](int x) { sum += x; });
    EXPECT_EQ(sum, 15);
}

// 分配器测试
TEST_F(VectorTest, AllocatorTest) {
    using TestAllocator = stl::allocator<int>;
    vector<int, TestAllocator> v;
    
    // 使用自定义分配器
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    
    // 获取分配器
    auto alloc = v.get_allocator();
    EXPECT_EQ(alloc, TestAllocator());
}

// 边界情况测试
TEST_F(VectorTest, EdgeCases) {
    vector<int> v;
    
    // 空vector的pop_back
    EXPECT_NO_THROW(v.pop_back());
    
    // 空vector的erase
    auto it = v.erase(v.begin(), v.end());
    EXPECT_EQ(it, v.end());
    
    // 空vector的clear
    EXPECT_NO_THROW(v.clear());
    
    // 大小为0的resize
    v.resize(0);
    EXPECT_TRUE(v.empty());
    
    // 插入空范围
    std::vector<int> empty;
    v.insert(v.end(), empty.begin(), empty.end());
    EXPECT_TRUE(v.empty());
}

// 性能相关测试
TEST_F(VectorTest, PerformanceRelated) {
    vector<int> v;
    
    // 测试自动扩容
    size_t old_capacity = v.capacity();
    for (int i = 0; i < 1000; ++i) {
        v.push_back(i);
        if (v.capacity() > old_capacity) {
            // 容量应该至少翻倍
            EXPECT_GE(v.capacity(), old_capacity * 2);
            old_capacity = v.capacity();
        }
    }
    
    // 测试shrink_to_fit
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), v.size());
}

// 复杂类型测试
TEST_F(VectorTest, ComplexTypes) {
    vector<std::string> v;
    
    // 字符串操作
    v.push_back("hello");
    v.push_back("world");
    v.emplace_back("test");
    
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], "hello");
    EXPECT_EQ(v[1], "world");
    EXPECT_EQ(v[2], "test");
    
    // 修改字符串
    v[0] = "HELLO";
    EXPECT_EQ(v[0], "HELLO");
    
    // 字符串移动
    std::string s = "move_test";
    v.push_back(std::move(s));
    EXPECT_EQ(v[3], "move_test");
    EXPECT_TRUE(s.empty());
}