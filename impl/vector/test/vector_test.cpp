#include <gtest/gtest.h>
#include <memory>
#include <type_traits>

#include "../vector.hpp"

// 测试基本构造和析构
TEST(VectorTest, BasicConstruction) {
    my::vector<int> v1;
    EXPECT_TRUE(v1.empty());
    EXPECT_EQ(v1.size(), 0);
    EXPECT_EQ(v1.capacity(), 0);
    
    my::vector<int> v2(5, 10);
    EXPECT_FALSE(v2.empty());
    EXPECT_EQ(v2.size(), 5);
    EXPECT_EQ(v2.capacity(), 5);
    EXPECT_EQ(v2[0], 10);
    EXPECT_EQ(v2[4], 10);
    
    my::vector<int> v3(v2);
    EXPECT_EQ(v3.size(), 5);
    EXPECT_EQ(v3[0], 10);
    EXPECT_EQ(v3[4], 10);
}

// 测试移动构造
TEST(VectorTest, MoveConstruction) {
    my::vector<int> v1(3, 42);
    my::vector<int> v2 = std::move(v1);
    
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2[0], 42);
    EXPECT_TRUE(v1.empty());
    EXPECT_EQ(v1.capacity(), 0);
}

// 测试移动赋值
TEST(VectorTest, MoveAssignment) {
    my::vector<int> v1(3, 42);
    my::vector<int> v2(5, 100);
    
    v2 = std::move(v1);
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2[0], 42);
    EXPECT_TRUE(v1.empty());
}

// 测试拷贝构造
TEST(VectorTest, CopyConstruction) {
    my::vector<int> v1(3, 42);
    my::vector<int> v2(v1);
    
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2[0], 42);
    EXPECT_EQ(v1.size(), 3);
    EXPECT_EQ(v1[0], 42);
}

// 测试拷贝赋值
TEST(VectorTest, CopyAssignment) {
    my::vector<int> v1(3, 42);
    my::vector<int> v2(5, 100);
    
    v2 = v1;
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2[0], 42);
    EXPECT_EQ(v1.size(), 3);
    EXPECT_EQ(v1[0], 42);
}

// 测试push_back和pop_back
TEST(VectorTest, PushBackAndPopBack) {
    my::vector<int> v;
    
    // 测试push_back
    v.push_back(1);
    EXPECT_EQ(v.size(), 1);
    EXPECT_EQ(v[0], 1);
    EXPECT_GE(v.capacity(), 1);
    
    v.push_back(2);
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[1], 2);
    
    v.push_back(3);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[2], 3);
    
    // 测试pop_back
    v.pop_back();
    EXPECT_EQ(v.size(), 2);
    
    v.pop_back();
    EXPECT_EQ(v.size(), 1);
    
    v.pop_back();
    EXPECT_TRUE(v.empty());
}

// 测试容量管理
TEST(VectorTest, CapacityManagement) {
    my::vector<int> v;
    
    EXPECT_EQ(v.capacity(), 0);
    
    v.reserve(10);
    EXPECT_EQ(v.capacity(), 10);
    EXPECT_EQ(v.size(), 0);
    
    v.push_back(1);
    v.push_back(2);
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v.capacity(), 10);
    
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), 2);
    EXPECT_EQ(v.size(), 2);
}

// 测试resize操作
TEST(VectorTest, ResizeOperation) {
    my::vector<int> v;
    
    v.resize(5, 10);
    EXPECT_EQ(v.size(), 5);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[4], 10);
    
    v.resize(3);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 10);
    
    v.resize(6, 20);
    EXPECT_EQ(v.size(), 6);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[5], 20);
}

// 测试clear操作
TEST(VectorTest, ClearOperation) {
    my::vector<int> v(5, 10);
    
    EXPECT_FALSE(v.empty());
    EXPECT_EQ(v.size(), 5);
    
    v.clear();
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
    EXPECT_GT(v.capacity(), 0);  // 容量可能保持不变
}

// 测试访问操作
TEST(VectorTest, AccessOperations) {
    my::vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
    EXPECT_EQ(v[2], 30);
    
    v[0] = 15;
    EXPECT_EQ(v[0], 15);
    
    EXPECT_EQ(v.at(1), 20);
    EXPECT_THROW(v.at(10), std::out_of_range);
    
    EXPECT_EQ(v.front(), 15);
    EXPECT_EQ(v.back(), 30);
}

// 测试数据访问
TEST(VectorTest, DataAccess) {
    my::vector<int> v;
    v.push_back(10);
    v.push_back(20);
    
    int* data = v.data();
    EXPECT_EQ(data[0], 10);
    EXPECT_EQ(data[1], 20);
    
    data[0] = 15;
    EXPECT_EQ(v[0], 15);
}

// 测试迭代器
TEST(VectorTest, Iterators) {
    my::vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    
    // 前向迭代器
    std::vector<int> result;
    for (auto it = v.begin(); it != v.end(); ++it) {
        result.push_back(*it);
    }
    EXPECT_EQ(result, std::vector<int>({10, 20, 30}));
    
    // 反向迭代器
    std::vector<int> reverse_result;
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
        reverse_result.push_back(*it);
    }
    EXPECT_EQ(reverse_result, std::vector<int>({30, 20, 10}));
    
    // const迭代器
    const my::vector<int>& cv = v;
    std::vector<int> const_result;
    for (auto it = cv.begin(); it != cv.end(); ++it) {
        const_result.push_back(*it);
    }
    EXPECT_EQ(const_result, std::vector<int>({10, 20, 30}));
}

// 测试insert操作
TEST(VectorTest, InsertOperation) {
    my::vector<int> v;
    v.push_back(10);
    v.push_back(30);
    
    // 在中间插入
    auto it = v.insert(v.cbegin() + 1, 20);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
    EXPECT_EQ(v[2], 30);
    EXPECT_EQ(*it, 20);
    
    // 在开头插入
    it = v.insert(v.cbegin(), 5);
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v[0], 5);
    EXPECT_EQ(*it, 5);
    
    // 在末尾插入
    it = v.insert(v.cend(), 35);
    EXPECT_EQ(v.size(), 5);
    EXPECT_EQ(v[4], 35);
    EXPECT_EQ(*it, 35);
}

// 测试erase操作
TEST(VectorTest, EraseOperation) {
    my::vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    v.push_back(40);
    
    // 删除中间元素
    auto it = v.erase(v.cbegin() + 1);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 30);
    EXPECT_EQ(v[2], 40);
    EXPECT_EQ(*it, 30);
    
    // 删除末尾元素
    it = v.erase(v.cend() - 1);
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 30);
    EXPECT_EQ(it, v.end());
    
    // 删除开头元素
    it = v.erase(v.cbegin());
    EXPECT_EQ(v.size(), 1);
    EXPECT_EQ(v[0], 30);
    EXPECT_EQ(*it, 30);
}

// 测试swap操作
TEST(VectorTest, SwapOperation) {
    my::vector<int> v1(3, 10);
    my::vector<int> v2(5, 20);
    
    v1.swap(v2);
    EXPECT_EQ(v1.size(), 5);
    EXPECT_EQ(v1[0], 20);
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2[0], 10);
}

// 测试边界条件
TEST(VectorTest, EdgeCases) {
    my::vector<int> v;
    
    // 空vector操作
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
    
    // 大vector操作
    v.resize(1000, 42);
    EXPECT_EQ(v.size(), 1000);
    EXPECT_EQ(v[0], 42);
    EXPECT_EQ(v[999], 42);
    
    // 清空后操作
    v.clear();
    EXPECT_TRUE(v.empty());
    
    // 重新填充
    for (int i = 0; i < 10; ++i) {
        v.push_back(i);
    }
    EXPECT_EQ(v.size(), 10);
}

// 测试异常安全
TEST(VectorTest, ExceptionSafety) {
    my::vector<int> v(5, 10);
    
    // 测试at方法的异常
    EXPECT_THROW(v.at(10), std::out_of_range);
    
    // 测试reserve的异常安全
    try {
        v.reserve(SIZE_MAX);
    } catch (const std::bad_alloc&) {
        // 预期的异常
    }
}

// 测试类型特征
TEST(VectorTest, TypeTraits) {
    EXPECT_TRUE(std::is_copy_constructible_v<my::vector<int>>);
    EXPECT_TRUE(std::is_copy_assignable_v<my::vector<int>>);
    EXPECT_TRUE(std::is_move_constructible_v<my::vector<int>>);
    EXPECT_TRUE(std::is_move_assignable_v<my::vector<int>>);
}

// 测试扩容策略
TEST(VectorTest, GrowthStrategy) {
    my::vector<int> v;
    
    // 测试容量增长
    size_t old_capacity = v.capacity();
    for (int i = 0; i < 100; ++i) {
        v.push_back(i);
        if (v.size() > old_capacity) {
            // 容量应该至少翻倍
            EXPECT_GE(v.capacity(), old_capacity * 2);
            old_capacity = v.capacity();
        }
    }
}

// 测试复杂类型
TEST(VectorTest, ComplexTypes) {
    my::vector<std::string> v;
    
    v.push_back("Hello");
    v.push_back("World");
    v.push_back("C++");
    
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], "Hello");
    EXPECT_EQ(v[1], "World");
    EXPECT_EQ(v[2], "C++");
    
    v.insert(v.cbegin() + 1, "Beautiful");
    EXPECT_EQ(v[1], "Beautiful");
    
    v.erase(v.cbegin() + 2);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[2], "C++");
}

// 测试emplace_back
TEST(VectorTest, EmplaceBack) {
    my::vector<std::pair<int, std::string>> v;
    
    v.emplace_back(1, "Hello");
    v.emplace_back(2, "World");
    
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[0].first, 1);
    EXPECT_EQ(v[0].second, "Hello");
    EXPECT_EQ(v[1].first, 2);
    EXPECT_EQ(v[1].second, "World");
}

// 测试构造函数初始化列表
TEST(VectorTest, InitializerList) {
    my::vector<int> v = {1, 2, 3, 4, 5};
    
    EXPECT_EQ(v.size(), 5);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[4], 5);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}