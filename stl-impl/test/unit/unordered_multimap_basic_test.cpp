#include <gtest/gtest.h>
#include <string>
#include <algorithm>
#include "stl/container/unordered_multimap.hpp"
#include "stl/functional.hpp"

using namespace stl;

class UnorderedMultiMapBasicTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// 基本构造和插入测试（现在支持真正的重复键）
TEST_F(UnorderedMultiMapBasicTest, BasicInsertAndFind) {
    unordered_multimap<int, std::string> map;
    
    // 基本插入
    auto result1 = map.insert({1, "one"});
    EXPECT_EQ(result1->first, 1);
    EXPECT_EQ(result1->second, "one");
    EXPECT_EQ(map.size(), 1);
    
    // 重复插入（现在支持真正的重复键）
    auto result2 = map.insert({1, "ONE"});
    EXPECT_EQ(result2->first, 1);
    // 现在支持重复插入，会有不同的值
    EXPECT_EQ(result2->second, "ONE");  // 新插入的值
    EXPECT_EQ(map.size(), 2);  // 大小会增加
    
    // 查找（会找到其中一个匹配的键）
    auto it = map.find(1);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->first, 1);
    // 可能找到"one"或"ONE"，取决于实现
    EXPECT_TRUE(it->second == "one" || it->second == "ONE");
    
    // 查找不存在的元素
    EXPECT_EQ(map.find(999), map.end());
}

// 重复键处理测试（现在支持真正的重复键）
TEST_F(UnorderedMultiMapBasicTest, DuplicateKeys) {
    unordered_multimap<int, std::string> map;
    
    // 插入相同键的不同值（现在支持重复键）
    map.insert({1, "first"});
    map.insert({1, "second"});  // 会插入重复键
    map.insert({1, "third"});   // 会插入重复键
    map.insert({2, "two"});
    map.insert({2, "double"});  // 会插入重复键
    
    EXPECT_EQ(map.size(), 5);  // 包含重复键
    
    // 检查键的个数（现在支持重复键）
    EXPECT_EQ(map.count(1), 3);  // 有3个键为1的元素
    EXPECT_EQ(map.count(2), 2);  // 有2个键为2的元素
    EXPECT_EQ(map.count(999), 0);
}

// 多元素操作测试
TEST_F(UnorderedMultiMapBasicTest, MultipleElements) {
    unordered_multimap<std::string, int> map;
    
    // 插入多个元素，包括重复键
    map.insert({"apple", 10});
    map.insert({"banana", 20});
    map.insert({"apple", 15});
    map.insert({"cherry", 30});
    map.insert({"banana", 25});
    map.insert({"apple", 12});
    
    EXPECT_EQ(map.size(), 6);
    EXPECT_EQ(map.count("apple"), 3);
    EXPECT_EQ(map.count("banana"), 2);
    EXPECT_EQ(map.count("cherry"), 1);
    
    // 验证所有键都存在
    EXPECT_NE(map.find("apple"), map.end());
    EXPECT_NE(map.find("banana"), map.end());
    EXPECT_NE(map.find("cherry"), map.end());
}

// 删除操作测试
TEST_F(UnorderedMultiMapBasicTest, EraseOperations) {
    unordered_multimap<int, std::string> map;
    
    // 插入一些重复键
    map.insert({5, "first"});
    map.insert({5, "second"});
    map.insert({5, "third"});
    map.insert({10, "ten"});
    map.insert({10, "double"});
    map.insert({15, "fifteen"});
    
    EXPECT_EQ(map.size(), 6);
    
    // 按键删除（应该删除所有匹配的元素）
    size_t erased_count = map.erase(5);
    EXPECT_EQ(erased_count, 3);
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.count(5), 0);
    
    // 按迭代器删除
    auto it = map.find(10);
    ASSERT_NE(it, map.end());
    map.erase(it);
    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map.count(10), 1);
}

// 清空容器测试
TEST_F(UnorderedMultiMapBasicTest, ClearOperations) {
    unordered_multimap<int, std::string> map;
    
    map.insert({1, "one"});
    map.insert({1, "ONE"});
    map.insert({2, "two"});
    map.insert({2, "TWO"});
    map.insert({2, "Two"});
    
    EXPECT_EQ(map.size(), 5);
    EXPECT_FALSE(map.empty());
    
    map.clear();
    
    EXPECT_EQ(map.size(), 0);
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.find(1), map.end());
    EXPECT_EQ(map.find(2), map.end());
}

// 迭代器测试
TEST_F(UnorderedMultiMapBasicTest, IteratorOperations) {
    unordered_multimap<int, std::string> map;
    
    map.insert({100, "hundred"});
    map.insert({100, "HUNDRED"});
    map.insert({200, "two hundred"});
    map.insert({300, "three hundred"});
    map.insert({200, "double"});
    
    // 遍历所有元素
    int count = 0;
    for (auto it = map.begin(); it != map.end(); ++it) {
        EXPECT_TRUE(it->first == 100 || it->first == 200 || it->first == 300);
        ++count;
    }
    EXPECT_EQ(count, 5);
    
    // 使用范围for循环
    count = 0;
    for (const auto& pair : map) {
        EXPECT_TRUE(pair.first == 100 || pair.first == 200 || pair.first == 300);
        ++count;
    }
    EXPECT_EQ(count, 5);
}

// 等价范围测试
TEST_F(UnorderedMultiMapBasicTest, EqualRange) {
    unordered_multimap<int, std::string> map;
    
    map.insert({42, "first"});
    map.insert({42, "second"});
    map.insert({42, "third"});
    map.insert({84, "eighty-four"});
    map.insert({84, "double"});
    
    // 测试等价范围
    auto range = map.equal_range(42);
    int count = 0;
    for (auto it = range.first; it != range.second; ++it) {
        if (equal_to<int>()(42, it->first)) {
            EXPECT_TRUE(it->second == "first" || it->second == "second" || it->second == "third");
            ++count;
        }
    }
    EXPECT_EQ(count, 3);
    EXPECT_NE(range.first, map.end());  // 第一个元素存在
    EXPECT_EQ(range.second, map.end());  // 第二个元素是end()
    
    // 测试不存在的键
    range = map.equal_range(999);
    EXPECT_EQ(range.first, map.end());
    EXPECT_EQ(range.second, map.end());
}

// 容量管理测试
TEST_F(UnorderedMultiMapBasicTest, CapacityManagement) {
    unordered_multimap<int, std::string> map;
    
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
    
    map.insert({1, "one"});
    map.insert({1, "ONE"});
    map.insert({2, "two"});
    
    EXPECT_FALSE(map.empty());
    EXPECT_EQ(map.size(), 3);
    EXPECT_GT(map.max_size(), 0);
}

// 哈希表操作测试
TEST_F(UnorderedMultiMapBasicTest, HashOperations) {
    unordered_multimap<int, std::string> map;
    
    // 插入足够多的元素以触发可能的重新哈希
    for (int i = 0; i < 20; ++i) {
        map.insert({i % 5, "value_" + std::to_string(i)});  // 创建重复键
    }
    
    EXPECT_EQ(map.size(), 20);
    EXPECT_GT(map.bucket_count(), 0);
    
    // 测试负载因子
    float load_factor = map.load_factor();
    EXPECT_GT(load_factor, 0.0f);
    
    // 测试最大负载因子
    float max_load_factor = map.max_load_factor();
    EXPECT_GT(max_load_factor, 0.0f);
}

// 字符串键测试
TEST_F(UnorderedMultiMapBasicTest, StringKeys) {
    unordered_multimap<std::string, int> map;
    
    map.insert({"apple", 10});
    map.insert({"banana", 20});
    map.insert({"apple", 15});
    map.insert({"cherry", 30});
    map.insert({"banana", 25});
    map.insert({"apple", 12});
    
    EXPECT_EQ(map.size(), 6);
    EXPECT_EQ(map.count("apple"), 3);
    EXPECT_EQ(map.count("banana"), 2);
    EXPECT_EQ(map.count("cherry"), 1);
    
    // 查找操作
    auto it = map.find("apple");
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->first, "apple");
    EXPECT_TRUE(it->second == 10 || it->second == 15 || it->second == 12);
    
    it = map.find("nonexistent");
    EXPECT_EQ(it, map.end());
}

// 移动语义测试
TEST_F(UnorderedMultiMapBasicTest, MoveSemantics) {
    unordered_multimap<int, std::string> map1;
    map1.insert({1, "move"});
    map1.insert({1, "MOVE"});
    map1.insert({2, "test"});
    
    unordered_multimap<int, std::string> map2 = std::move(map1);
    
    EXPECT_EQ(map2.size(), 3);
    EXPECT_EQ(map2.count(1), 2);
    EXPECT_EQ(map2.count(2), 1);
    
    // map1应该为空（移动后）
    EXPECT_EQ(map1.size(), 0);
}

// 交换操作测试
TEST_F(UnorderedMultiMapBasicTest, SwapOperations) {
    unordered_multimap<int, std::string> map1, map2;
    
    map1.insert({1, "one"});
    map1.insert({1, "ONE"});
    map1.insert({2, "two"});
    
    map2.insert({10, "ten"});
    map2.insert({20, "twenty"});
    
    map1.swap(map2);
    
    EXPECT_EQ(map1.size(), 2);
    EXPECT_EQ(map1.count(10), 1);
    EXPECT_EQ(map1.count(20), 1);
    
    EXPECT_EQ(map2.size(), 3);
    EXPECT_EQ(map2.count(1), 2);
    EXPECT_EQ(map2.count(2), 1);
}

// 复杂值类型测试
TEST_F(UnorderedMultiMapBasicTest, ComplexValueType) {
    unordered_multimap<int, std::vector<std::string>> map;
    
    map.insert({1, {"a", "b", "c"}});
    map.insert({1, {"x", "y", "z"}});
    map.insert({2, {"hello", "world"}});
    
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.count(1), 2);
    EXPECT_EQ(map.count(2), 1);
    
    auto it = map.find(1);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->second.size(), 3);
}

// Emplace操作测试
TEST_F(UnorderedMultiMapBasicTest, EmplaceOperations) {
    unordered_multimap<std::string, std::string> map;
    
    // 使用emplace构造键值对
    auto it1 = map.emplace("key1", "value1");
    EXPECT_EQ(it1->first, "key1");
    EXPECT_EQ(it1->second, "value1");
    
    // 重复键
    auto it2 = map.emplace("key1", "value2");
    EXPECT_EQ(it2->first, "key1");
    EXPECT_EQ(it2->second, "value2");
    
    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map.count("key1"), 2);
}