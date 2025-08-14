#include <gtest/gtest.h>
#include <string>
#include <algorithm>
#include "stl/container/unordered_map.hpp"

using namespace stl;

class UnorderedMapBasicTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// 基本构造和插入测试
TEST_F(UnorderedMapBasicTest, BasicInsertAndFind) {
    unordered_map<int, std::string> map;
    
    // 基本插入
    auto result1 = map.insert({1, "one"});
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(result1.first->first, 1);
    EXPECT_EQ(result1.first->second, "one");
    EXPECT_EQ(map.size(), 1);
    
    // 重复插入
    auto result2 = map.insert({1, "ONE"});
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(result2.first->first, 1);
    EXPECT_EQ(result2.first->second, "one");
    EXPECT_EQ(map.size(), 1);
    
    // 查找
    auto it = map.find(1);
    EXPECT_NE(it, map.end());
    EXPECT_EQ(it->first, 1);
    EXPECT_EQ(it->second, "one");
    
    // 不存在的键
    auto it2 = map.find(99);
    EXPECT_EQ(it2, map.end());
}

// operator[]测试
TEST_F(UnorderedMapBasicTest, OperatorAccess) {
    unordered_map<int, std::string> map;
    
    // operator[] - 不存在的键
    map[1] = "one";
    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map.at(1), "one");
    
    // operator[] - 已存在的键
    map[1] = "ONE";
    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map.at(1), "ONE");
    
    // operator[] - 新键
    map[2] = "two";
    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map.at(2), "two");
}

// 多个元素测试
TEST_F(UnorderedMapBasicTest, MultipleElements) {
    unordered_map<int, std::string> map;
    
    // 插入多个元素
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    
    EXPECT_EQ(map.size(), 3);
    
    // 验证所有元素
    auto it = map.find(1);
    EXPECT_NE(it, map.end());
    EXPECT_EQ(it->second, "one");
    
    it = map.find(2);
    EXPECT_NE(it, map.end());
    EXPECT_EQ(it->second, "two");
    
    it = map.find(3);
    EXPECT_NE(it, map.end());
    EXPECT_EQ(it->second, "three");
    
    // 验证不存在的元素
    EXPECT_EQ(map.count(0), 0);
    EXPECT_EQ(map.count(4), 0);
}

// 删除测试
TEST_F(UnorderedMapBasicTest, Erase) {
    unordered_map<int, std::string> map;
    
    // 插入元素
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    
    EXPECT_EQ(map.size(), 3);
    
    // 删除存在的键
    size_t count = map.erase(2);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map.count(2), 0);
    
    // 删除不存在的键
    count = map.erase(99);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(map.size(), 2);
    
    // 验证剩余元素
    EXPECT_EQ(map.count(1), 1);
    EXPECT_EQ(map.count(3), 1);
}

// 清空测试
TEST_F(UnorderedMapBasicTest, Clear) {
    unordered_map<int, std::string> map;
    
    // 插入元素
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    
    EXPECT_EQ(map.size(), 3);
    
    // 清空
    map.clear();
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
    EXPECT_EQ(map.count(1), 0);
    EXPECT_EQ(map.count(2), 0);
    EXPECT_EQ(map.count(3), 0);
}

// 迭代器测试
TEST_F(UnorderedMapBasicTest, Iterators) {
    unordered_map<int, std::string> map;
    
    // 空map迭代器
    EXPECT_EQ(map.begin(), map.end());
    
    // 插入元素
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    
    // 非空map迭代器
    EXPECT_NE(map.begin(), map.end());
    
    // 遍历元素
    std::vector<int> keys;
    std::vector<std::string> values;
    for (auto it = map.begin(); it != map.end(); ++it) {
        keys.push_back(it->first);
        values.push_back(it->second);
    }
    
    EXPECT_EQ(keys.size(), 3);
    EXPECT_TRUE(std::find(keys.begin(), keys.end(), 1) != keys.end());
    EXPECT_TRUE(std::find(keys.begin(), keys.end(), 2) != keys.end());
    EXPECT_TRUE(std::find(keys.begin(), keys.end(), 3) != keys.end());
    
    EXPECT_TRUE(std::find(values.begin(), values.end(), "one") != values.end());
    EXPECT_TRUE(std::find(values.begin(), values.end(), "two") != values.end());
    EXPECT_TRUE(std::find(values.begin(), values.end(), "three") != values.end());
}

// 字符串键测试
TEST_F(UnorderedMapBasicTest, StringKeys) {
    unordered_map<std::string, int> map;
    
    // 插入字符串键
    map.insert({"hello", 1});
    map.insert({"world", 2});
    map.insert({"test", 3});
    
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.at("hello"), 1);
    EXPECT_EQ(map.at("world"), 2);
    EXPECT_EQ(map.at("test"), 3);
    
    // 查找字符串
    auto it = map.find("hello");
    EXPECT_NE(it, map.end());
    EXPECT_EQ(it->first, "hello");
    EXPECT_EQ(it->second, 1);
    
    // 删除字符串
    map.erase("world");
    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map.count("world"), 0);
}

// 容量测试
TEST_F(UnorderedMapBasicTest, Capacity) {
    unordered_map<int, std::string> map;
    
    // 初始状态
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
    EXPECT_GT(map.bucket_count(), 0);
    
    // 添加元素
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    
    EXPECT_FALSE(map.empty());
    EXPECT_EQ(map.size(), 3);
    EXPECT_GT(map.load_factor(), 0.0f);
    
    // max_size
    EXPECT_GT(map.max_size(), 0);
}

// 自动扩容测试
TEST_F(UnorderedMapBasicTest, AutoRehash) {
    unordered_map<int, std::string> map;
    
    size_t initial_buckets = map.bucket_count();
    
    // 插入足够多的元素触发自动扩容
    for (int i = 0; i < 50; ++i) {
        map.insert({i, "value" + std::to_string(i)});
    }
    
    // 检查是否发生了扩容
    EXPECT_GT(map.bucket_count(), initial_buckets);
    EXPECT_EQ(map.size(), 50);
    
    // 验证所有元素仍然存在
    for (int i = 0; i < 50; ++i) {
        EXPECT_EQ(map.count(i), 1);
    }
}

// at()异常测试
TEST_F(UnorderedMapBasicTest, AtException) {
    unordered_map<int, std::string> map;
    
    map.insert({1, "one"});
    
    // 存在的键
    EXPECT_EQ(map.at(1), "one");
    
    // 不存在的键
    EXPECT_THROW(map.at(99), std::out_of_range);
    
    // 常量map的at()
    const auto& const_map = map;
    EXPECT_EQ(const_map.at(1), "one");
    EXPECT_THROW(const_map.at(99), std::out_of_range);
}