#include <gtest/gtest.h>
#include <string>
#include <algorithm>
#include "stl/hash/hash_table.hpp"
#include "stl/functional.hpp"

using namespace stl;

// 测试用的hash_table派生类，实现get_key方法
template <typename Key, typename Value, typename Hash = stl::hash<Key>, typename KeyEqual = stl::equal_to<Key>, typename Allocator = stl::allocator<Value>>
class test_hash_table : public hash_table<Key, Value, Hash, KeyEqual, Allocator> {
public:
    using base_type = hash_table<Key, Value, Hash, KeyEqual, Allocator>;
    using base_type::base_type;
    
protected:
    const Key& get_key(const Value& value) const override {
        return value; // 对于Value=Key的情况，直接返回value
    }
};

class HashTableBasicTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// 基本构造和插入测试
TEST_F(HashTableBasicTest, BasicInsertAndFind) {
    test_hash_table<int, int> table;
    
    // 基本插入
    auto result1 = table.insert(1);
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(*result1.first, 1);
    EXPECT_EQ(table.size(), 1);
    
    // 重复插入
    auto result2 = table.insert(1);
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(*result2.first, 1);
    EXPECT_EQ(table.size(), 1);
    
    // 查找
    auto it = table.find(1);
    EXPECT_NE(it, table.end());
    EXPECT_EQ(*it, 1);
    
    // 不存在的键
    auto it2 = table.find(99);
    EXPECT_EQ(it2, table.end());
}

// 多个元素测试
TEST_F(HashTableBasicTest, MultipleElements) {
    test_hash_table<int, int> table;
    
    // 插入多个元素
    for (int i = 1; i <= 10; ++i) {
        auto result = table.insert(i);
        EXPECT_TRUE(result.second);
        EXPECT_EQ(*result.first, i);
    }
    
    EXPECT_EQ(table.size(), 10);
    
    // 验证所有元素
    for (int i = 1; i <= 10; ++i) {
        auto it = table.find(i);
        EXPECT_NE(it, table.end());
        EXPECT_EQ(*it, i);
        EXPECT_EQ(table.count(i), 1);
    }
    
    // 验证不存在的元素
    EXPECT_EQ(table.count(0), 0);
    EXPECT_EQ(table.count(11), 0);
}

// 删除测试
TEST_F(HashTableBasicTest, Erase) {
    test_hash_table<int, int> table;
    
    // 插入元素
    table.insert(1);
    table.insert(2);
    table.insert(3);
    
    EXPECT_EQ(table.size(), 3);
    
    // 删除存在的键
    size_t count = table.erase(2);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(table.size(), 2);
    EXPECT_EQ(table.count(2), 0);
    
    // 删除不存在的键
    count = table.erase(99);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(table.size(), 2);
    
    // 验证剩余元素
    EXPECT_EQ(table.count(1), 1);
    EXPECT_EQ(table.count(3), 1);
}

// 清空测试
TEST_F(HashTableBasicTest, Clear) {
    test_hash_table<int, int> table;
    
    // 插入元素
    table.insert(1);
    table.insert(2);
    table.insert(3);
    
    EXPECT_EQ(table.size(), 3);
    
    // 清空
    table.clear();
    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.size(), 0);
    EXPECT_EQ(table.count(1), 0);
    EXPECT_EQ(table.count(2), 0);
    EXPECT_EQ(table.count(3), 0);
}

// 迭代器测试
TEST_F(HashTableBasicTest, Iterators) {
    test_hash_table<int, int> table;
    
    // 空表迭代器
    EXPECT_EQ(table.begin(), table.end());
    
    // 插入元素
    table.insert(1);
    table.insert(2);
    table.insert(3);
    
    // 非空表迭代器
    EXPECT_NE(table.begin(), table.end());
    
    // 遍历元素
    std::vector<int> elements;
    for (auto it = table.begin(); it != table.end(); ++it) {
        elements.push_back(*it);
    }
    
    EXPECT_EQ(elements.size(), 3);
    EXPECT_TRUE(std::find(elements.begin(), elements.end(), 1) != elements.end());
    EXPECT_TRUE(std::find(elements.begin(), elements.end(), 2) != elements.end());
    EXPECT_TRUE(std::find(elements.begin(), elements.end(), 3) != elements.end());
}

// 字符串测试
TEST_F(HashTableBasicTest, StringKeys) {
    test_hash_table<std::string, std::string> table;
    
    // 插入字符串
    table.insert("hello");
    table.insert("world");
    table.insert("test");
    
    EXPECT_EQ(table.size(), 3);
    EXPECT_EQ(table.count("hello"), 1);
    EXPECT_EQ(table.count("world"), 1);
    EXPECT_EQ(table.count("test"), 1);
    
    // 查找字符串
    auto it = table.find("hello");
    EXPECT_NE(it, table.end());
    EXPECT_EQ(*it, "hello");
    
    // 删除字符串
    table.erase("world");
    EXPECT_EQ(table.size(), 2);
    EXPECT_EQ(table.count("world"), 0);
}

// 容量测试
TEST_F(HashTableBasicTest, Capacity) {
    test_hash_table<int, int> table;
    
    // 初始状态
    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.size(), 0);
    EXPECT_GT(table.bucket_count(), 0);
    
    // 添加元素
    table.insert(1);
    table.insert(2);
    table.insert(3);
    
    EXPECT_FALSE(table.empty());
    EXPECT_EQ(table.size(), 3);
    EXPECT_GT(table.load_factor(), 0.0f);
    
    // max_size
    EXPECT_GT(table.max_size(), 0);
}

// 自动扩容测试
TEST_F(HashTableBasicTest, AutoRehash) {
    test_hash_table<int, int> table;
    
    size_t initial_buckets = table.bucket_count();
    
    // 插入足够多的元素触发自动扩容
    for (int i = 0; i < 50; ++i) {
        table.insert(i);
    }
    
    // 检查是否发生了扩容
    EXPECT_GT(table.bucket_count(), initial_buckets);
    EXPECT_EQ(table.size(), 50);
    
    // 验证所有元素仍然存在
    for (int i = 0; i < 50; ++i) {
        EXPECT_EQ(table.count(i), 1);
    }
}