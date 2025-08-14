#include <gtest/gtest.h>
#include <string>
#include <algorithm>
#include "stl/container/unordered_set.hpp"

using namespace stl;

class UnorderedSetBasicTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// 基本构造和插入测试
TEST_F(UnorderedSetBasicTest, BasicInsertAndFind) {
    unordered_set<int> set;
    
    // 基本插入
    auto result1 = set.insert(1);
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(*result1.first, 1);
    EXPECT_EQ(set.size(), 1);
    
    // 重复插入
    auto result2 = set.insert(1);
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(*result2.first, 1);
    EXPECT_EQ(set.size(), 1);
    
    // 查找
    auto it = set.find(1);
    EXPECT_NE(it, set.end());
    EXPECT_EQ(*it, 1);
    
    // 不存在的键
    auto it2 = set.find(99);
    EXPECT_EQ(it2, set.end());
}

// 多个元素测试
TEST_F(UnorderedSetBasicTest, MultipleElements) {
    unordered_set<int> set;
    
    // 插入多个元素
    for (int i = 1; i <= 10; ++i) {
        auto result = set.insert(i);
        EXPECT_TRUE(result.second);
        EXPECT_EQ(*result.first, i);
    }
    
    EXPECT_EQ(set.size(), 10);
    
    // 验证所有元素
    for (int i = 1; i <= 10; ++i) {
        auto it = set.find(i);
        EXPECT_NE(it, set.end());
        EXPECT_EQ(*it, i);
        EXPECT_EQ(set.count(i), 1);
    }
    
    // 验证不存在的元素
    EXPECT_EQ(set.count(0), 0);
    EXPECT_EQ(set.count(11), 0);
}

// 删除测试
TEST_F(UnorderedSetBasicTest, Erase) {
    unordered_set<int> set;
    
    // 插入元素
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    EXPECT_EQ(set.size(), 3);
    
    // 删除存在的键
    size_t count = set.erase(2);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(set.size(), 2);
    EXPECT_EQ(set.count(2), 0);
    
    // 删除不存在的键
    count = set.erase(99);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(set.size(), 2);
    
    // 验证剩余元素
    EXPECT_EQ(set.count(1), 1);
    EXPECT_EQ(set.count(3), 1);
}

// 清空测试
TEST_F(UnorderedSetBasicTest, Clear) {
    unordered_set<int> set;
    
    // 插入元素
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    EXPECT_EQ(set.size(), 3);
    
    // 清空
    set.clear();
    EXPECT_TRUE(set.empty());
    EXPECT_EQ(set.size(), 0);
    EXPECT_EQ(set.count(1), 0);
    EXPECT_EQ(set.count(2), 0);
    EXPECT_EQ(set.count(3), 0);
}

// 迭代器测试
TEST_F(UnorderedSetBasicTest, Iterators) {
    unordered_set<int> set;
    
    // 空集合迭代器
    EXPECT_EQ(set.begin(), set.end());
    
    // 插入元素
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    // 非空集合迭代器
    EXPECT_NE(set.begin(), set.end());
    
    // 遍历元素
    std::vector<int> elements;
    for (auto it = set.begin(); it != set.end(); ++it) {
        elements.push_back(*it);
    }
    
    EXPECT_EQ(elements.size(), 3);
    EXPECT_TRUE(std::find(elements.begin(), elements.end(), 1) != elements.end());
    EXPECT_TRUE(std::find(elements.begin(), elements.end(), 2) != elements.end());
    EXPECT_TRUE(std::find(elements.begin(), elements.end(), 3) != elements.end());
}

// 字符串测试
TEST_F(UnorderedSetBasicTest, StringKeys) {
    unordered_set<std::string> set;
    
    // 插入字符串
    set.insert("hello");
    set.insert("world");
    set.insert("test");
    
    EXPECT_EQ(set.size(), 3);
    EXPECT_EQ(set.count("hello"), 1);
    EXPECT_EQ(set.count("world"), 1);
    EXPECT_EQ(set.count("test"), 1);
    
    // 查找字符串
    auto it = set.find("hello");
    EXPECT_NE(it, set.end());
    EXPECT_EQ(*it, "hello");
    
    // 删除字符串
    set.erase("world");
    EXPECT_EQ(set.size(), 2);
    EXPECT_EQ(set.count("world"), 0);
}

// 容量测试
TEST_F(UnorderedSetBasicTest, Capacity) {
    unordered_set<int> set;
    
    // 初始状态
    EXPECT_TRUE(set.empty());
    EXPECT_EQ(set.size(), 0);
    EXPECT_GT(set.bucket_count(), 0);
    
    // 添加元素
    set.insert(1);
    set.insert(2);
    set.insert(3);
    
    EXPECT_FALSE(set.empty());
    EXPECT_EQ(set.size(), 3);
    EXPECT_GT(set.load_factor(), 0.0f);
    
    // max_size
    EXPECT_GT(set.max_size(), 0);
}

// 自动扩容测试
TEST_F(UnorderedSetBasicTest, AutoRehash) {
    unordered_set<int> set;
    
    size_t initial_buckets = set.bucket_count();
    
    // 插入足够多的元素触发自动扩容
    for (int i = 0; i < 50; ++i) {
        set.insert(i);
    }
    
    // 检查是否发生了扩容
    EXPECT_GT(set.bucket_count(), initial_buckets);
    EXPECT_EQ(set.size(), 50);
    
    // 验证所有元素仍然存在
    for (int i = 0; i < 50; ++i) {
        EXPECT_EQ(set.count(i), 1);
    }
}