#include <gtest/gtest.h>
#include <string>
#include <memory>
#include <algorithm>
#include <initializer_list>
#include <utility>
#include "stl/container/unordered_map.hpp"

using namespace stl;

class UnorderedMapTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// 构造函数测试
TEST_F(UnorderedMapTest, Constructors) {
    // 默认构造函数
    unordered_map<int, std::string> map1;
    EXPECT_TRUE(map1.empty());
    EXPECT_EQ(map1.size(), 0);
    EXPECT_GT(map1.bucket_count(), 0);
    
    // 指定桶数构造函数
    unordered_map<int, std::string> map2(10);
    EXPECT_TRUE(map2.empty());
    EXPECT_EQ(map2.size(), 0);
    EXPECT_GE(map2.bucket_count(), 10);
    
    // 迭代器构造函数
    std::vector<std::pair<int, std::string>> vec = {{1, "one"}, {2, "two"}, {3, "three"}};
    unordered_map<int, std::string> map3(vec.begin(), vec.end());
    EXPECT_EQ(map3.size(), 3);
    EXPECT_EQ(map3.at(1), "one");
    EXPECT_EQ(map3.at(2), "two");
    EXPECT_EQ(map3.at(3), "three");
    
    // 初始化列表构造函数
    unordered_map<int, std::string> map4 = {{1, "one"}, {2, "two"}, {3, "three"}};
    EXPECT_EQ(map4.size(), 3);
    EXPECT_EQ(map4.at(1), "one");
    EXPECT_EQ(map4.at(2), "two");
    EXPECT_EQ(map4.at(3), "three");
    
    // 拷贝构造函数
    unordered_map<int, std::string> map5(map4);
    EXPECT_EQ(map5.size(), 3);
    EXPECT_EQ(map5.at(1), "one");
    EXPECT_EQ(map5.at(2), "two");
    EXPECT_EQ(map5.at(3), "three");
    
    // 移动构造函数
    unordered_map<int, std::string> map6(std::move(map5));
    EXPECT_EQ(map6.size(), 3);
    EXPECT_TRUE(map5.empty());
}

// 赋值运算符测试
TEST_F(UnorderedMapTest, AssignmentOperators) {
    unordered_map<int, std::string> map1 = {{1, "one"}, {2, "two"}, {3, "three"}};
    
    // 拷贝赋值
    unordered_map<int, std::string> map2;
    map2 = map1;
    EXPECT_EQ(map2.size(), 3);
    EXPECT_EQ(map2.at(1), "one");
    EXPECT_EQ(map2.at(2), "two");
    EXPECT_EQ(map2.at(3), "three");
    
    // 移动赋值
    unordered_map<int, std::string> map3;
    map3 = std::move(map2);
    EXPECT_EQ(map3.size(), 3);
    EXPECT_TRUE(map2.empty());
    
    // 初始化列表赋值
    unordered_map<int, std::string> map4;
    map4 = {{4, "four"}, {5, "five"}};
    EXPECT_EQ(map4.size(), 2);
    EXPECT_EQ(map4.at(4), "four");
    EXPECT_EQ(map4.at(5), "five");
}

// 元素访问测试
TEST_F(UnorderedMapTest, ElementAccess) {
    unordered_map<int, std::string> map;
    
    // operator[] - 不存在的键
    map[1] = "one";
    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map.at(1), "one");
    
    // operator[] - 已存在的键
    map[1] = "ONE";
    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map.at(1), "ONE");
    
    // operator[] - 移动语义
    std::string str = "two";
    map[2] = std::move(str);
    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map.at(2), "two");
    EXPECT_TRUE(str.empty());
    
    // at() - 存在的键
    EXPECT_EQ(map.at(1), "ONE");
    EXPECT_EQ(map.at(2), "two");
    
    // at() - 不存在的键
    EXPECT_THROW(map.at(3), std::out_of_range);
    
    // 常量at()
    const auto& const_map = map;
    EXPECT_EQ(const_map.at(1), "ONE");
    EXPECT_THROW(const_map.at(3), std::out_of_range);
}

// 插入测试
TEST_F(UnorderedMapTest, Insert) {
    unordered_map<int, std::string> map;
    
    // 插入单个元素
    auto result1 = map.insert({1, "one"});
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(result1.first->first, 1);
    EXPECT_EQ(result1.first->second, "one");
    EXPECT_EQ(map.size(), 1);
    
    // 插入重复元素
    auto result2 = map.insert({1, "ONE"});
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(result2.first->first, 1);
    EXPECT_EQ(result2.first->second, "one");
    EXPECT_EQ(map.size(), 1);
    
    // 插入多个元素
    map.insert({2, "two"});
    map.insert({3, "three"});
    map.insert({4, "four"});
    
    EXPECT_EQ(map.size(), 4);
    EXPECT_EQ(map.at(1), "one");
    EXPECT_EQ(map.at(2), "two");
    EXPECT_EQ(map.at(3), "three");
    EXPECT_EQ(map.at(4), "four");
    
    // 使用hint插入
    auto it = map.insert(map.begin(), {5, "five"});
    EXPECT_EQ(it->first, 5);
    EXPECT_EQ(it->second, "five");
    EXPECT_EQ(map.size(), 5);
    EXPECT_EQ(map.at(5), "five");
    
    // 迭代器范围插入
    std::vector<std::pair<int, std::string>> vec = {{6, "six"}, {7, "seven"}};
    map.insert(vec.begin(), vec.end());
    EXPECT_EQ(map.size(), 7);
    EXPECT_EQ(map.at(6), "six");
    EXPECT_EQ(map.at(7), "seven");
    
    // 初始化列表插入
    map.insert({{8, "eight"}, {9, "nine"}});
    EXPECT_EQ(map.size(), 9);
    EXPECT_EQ(map.at(8), "eight");
    EXPECT_EQ(map.at(9), "nine");
}

// emplace测试
TEST_F(UnorderedMapTest, Emplace) {
    unordered_map<int, std::string> map;
    
    // emplace插入
    auto result1 = map.emplace(1, "one");
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(result1.first->first, 1);
    EXPECT_EQ(result1.first->second, "one");
    EXPECT_EQ(map.size(), 1);
    
    // emplace重复元素
    auto result2 = map.emplace(1, "ONE");
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(result2.first->first, 1);
    EXPECT_EQ(result2.first->second, "one");
    EXPECT_EQ(map.size(), 1);
    
    // emplace_hint
    auto it = map.emplace_hint(map.begin(), 2, "two");
    EXPECT_EQ(it->first, 2);
    EXPECT_EQ(it->second, "two");
    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map.at(2), "two");
    
    // emplace更多元素
    map.emplace(3, "three");
    map.emplace(4, "four");
    
    EXPECT_EQ(map.size(), 4);
    EXPECT_EQ(map.at(1), "one");
    EXPECT_EQ(map.at(2), "two");
    EXPECT_EQ(map.at(3), "three");
    EXPECT_EQ(map.at(4), "four");
}

// try_emplace测试
TEST_F(UnorderedMapTest, TryEmplace) {
    unordered_map<int, std::string> map;
    
    // try_emplace - 不存在的键
    auto result1 = map.try_emplace(1, "one");
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(result1.first->first, 1);
    EXPECT_EQ(result1.first->second, "one");
    EXPECT_EQ(map.size(), 1);
    
    // try_emplace - 已存在的键
    auto result2 = map.try_emplace(1, "ONE");
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(result2.first->first, 1);
    EXPECT_EQ(result2.first->second, "one");
    EXPECT_EQ(map.size(), 1);
    
    // try_emplace_hint
    auto it = map.try_emplace(map.begin(), 2, "two");
    EXPECT_EQ(it->first, 2);
    EXPECT_EQ(it->second, "two");
    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map.at(2), "two");
    
    // try_emplace - 移动语义
    std::string str = "three";
    auto result3 = map.try_emplace(3, std::move(str));
    EXPECT_TRUE(result3.second);
    EXPECT_EQ(result3.first->second, "three");
    EXPECT_TRUE(str.empty());
}

// insert_or_assign测试
TEST_F(UnorderedMapTest, InsertOrAssign) {
    unordered_map<int, std::string> map;
    
    // insert_or_assign - 不存在的键
    auto result1 = map.insert_or_assign(1, "one");
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(result1.first->first, 1);
    EXPECT_EQ(result1.first->second, "one");
    EXPECT_EQ(map.size(), 1);
    
    // insert_or_assign - 已存在的键
    auto result2 = map.insert_or_assign(1, "ONE");
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(result2.first->first, 1);
    EXPECT_EQ(result2.first->second, "ONE");
    EXPECT_EQ(map.size(), 1);
    
    // insert_or_assign_hint
    auto it = map.insert_or_assign(map.begin(), 2, "two");
    EXPECT_EQ(it->first, 2);
    EXPECT_EQ(it->second, "two");
    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map.at(2), "two");
    
    // insert_or_assign - 移动语义
    std::string str = "three";
    auto result3 = map.insert_or_assign(3, std::move(str));
    EXPECT_TRUE(result3.second);
    EXPECT_EQ(result3.first->second, "three");
    EXPECT_TRUE(str.empty());
}

// 查找测试
TEST_F(UnorderedMapTest, Find) {
    unordered_map<int, std::string> map;
    
    // 空map查找
    auto it1 = map.find(1);
    EXPECT_EQ(it1, map.end());
    
    // 插入元素后查找
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    
    auto it2 = map.find(1);
    EXPECT_NE(it2, map.end());
    EXPECT_EQ(it2->first, 1);
    EXPECT_EQ(it2->second, "one");
    
    auto it3 = map.find(2);
    EXPECT_NE(it3, map.end());
    EXPECT_EQ(it3->first, 2);
    EXPECT_EQ(it3->second, "two");
    
    auto it4 = map.find(99);
    EXPECT_EQ(it4, map.end());
}

// count测试
TEST_F(UnorderedMapTest, Count) {
    unordered_map<int, std::string> map;
    
    // 空map计数
    EXPECT_EQ(map.count(1), 0);
    
    // 插入元素后计数
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    
    EXPECT_EQ(map.count(1), 1);
    EXPECT_EQ(map.count(2), 1);
    EXPECT_EQ(map.count(3), 1);
    EXPECT_EQ(map.count(99), 0);
}

// equal_range测试
TEST_F(UnorderedMapTest, EqualRange) {
    unordered_map<int, std::string> map;
    
    // 空map范围
    auto range1 = map.equal_range(1);
    EXPECT_EQ(range1.first, map.end());
    EXPECT_EQ(range1.second, map.end());
    
    // 插入元素后范围
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    
    auto range2 = map.equal_range(1);
    EXPECT_NE(range2.first, map.end());
    EXPECT_EQ(range2.first->first, 1);
    EXPECT_EQ(range2.first->second, "one");
    auto next = range2.first;
    ++next;
    EXPECT_EQ(range2.second, next);
    
    auto range3 = map.equal_range(99);
    EXPECT_EQ(range3.first, map.end());
    EXPECT_EQ(range3.second, map.end());
}

// 删除测试
TEST_F(UnorderedMapTest, Erase) {
    unordered_map<int, std::string> map;
    
    // 插入元素
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    map.insert({4, "four"});
    map.insert({5, "five"});
    
    EXPECT_EQ(map.size(), 5);
    
    // 通过迭代器删除
    auto it = map.find(3);
    auto next_it = map.erase(it);
    EXPECT_EQ(map.size(), 4);
    EXPECT_EQ(map.count(3), 0);
    EXPECT_NE(next_it, map.end());
    
    // 通过key删除
    size_t count = map.erase(2);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.count(2), 0);
    
    // 删除不存在的key
    count = map.erase(99);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(map.size(), 3);
    
    // 删除范围
    auto first = map.begin();
    auto last = map.end();
    map.erase(first, last);
    EXPECT_TRUE(map.empty());
}

// clear测试
TEST_F(UnorderedMapTest, Clear) {
    unordered_map<int, std::string> map;
    
    // 插入元素
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    
    EXPECT_EQ(map.size(), 3);
    
    // 清空map
    map.clear();
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
    EXPECT_EQ(map.count(1), 0);
    EXPECT_EQ(map.count(2), 0);
    EXPECT_EQ(map.count(3), 0);
}

// 容量测试
TEST_F(UnorderedMapTest, Capacity) {
    unordered_map<int, std::string> map;
    
    // 初始状态
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
    EXPECT_GT(map.bucket_count(), 0);
    EXPECT_EQ(map.load_factor(), 0.0f);
    
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

// 桶相关测试
TEST_F(UnorderedMapTest, BucketOperations) {
    unordered_map<int, std::string> map;
    
    // 初始桶数
    size_t initial_buckets = map.bucket_count();
    EXPECT_GT(initial_buckets, 0);
    
    // max_bucket_count
    EXPECT_GT(map.max_bucket_count(), 0);
    
    // 添加元素
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    
    // 检查负载因子
    float load_factor = map.load_factor();
    EXPECT_GT(load_factor, 0.0f);
    EXPECT_LE(load_factor, 1.0f);
    
    // 检查桶索引
    size_t bucket_idx = map.bucket(1);
    EXPECT_LT(bucket_idx, map.bucket_count());
}

// max_load_factor测试
TEST_F(UnorderedMapTest, MaxLoadFactor) {
    unordered_map<int, std::string> map;
    
    // 默认最大负载因子
    float default_max_load = map.max_load_factor();
    EXPECT_GT(default_max_load, 0.0f);
    
    // 设置新的最大负载因子
    map.max_load_factor(0.5f);
    EXPECT_EQ(map.max_load_factor(), 0.5f);
}

// rehash测试
TEST_F(UnorderedMapTest, Rehash) {
    unordered_map<int, std::string> map;
    
    // 插入一些元素
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    
    size_t old_bucket_count = map.bucket_count();
    
    // rehash到更大的桶数
    map.rehash(20);
    EXPECT_GE(map.bucket_count(), 20);
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.at(1), "one");
    EXPECT_EQ(map.at(2), "two");
    EXPECT_EQ(map.at(3), "three");
    
    // rehash到更小的桶数（应该不会减小）
    map.rehash(5);
    EXPECT_GE(map.bucket_count(), 5);
    EXPECT_EQ(map.size(), 3);
}

// reserve测试
TEST_F(UnorderedMapTest, Reserve) {
    unordered_map<int, std::string> map;
    
    // reserve空间
    map.reserve(100);
    EXPECT_GE(map.bucket_count(), 100);
    
    // 插入元素
    for (int i = 0; i < 50; ++i) {
        map.insert({i, "value" + std::to_string(i)});
    }
    
    EXPECT_EQ(map.size(), 50);
    EXPECT_LE(map.load_factor(), map.max_load_factor());
}

// 迭代器测试
TEST_F(UnorderedMapTest, Iterators) {
    unordered_map<int, std::string> map;
    
    // 空map迭代器
    EXPECT_EQ(map.begin(), map.end());
    EXPECT_EQ(map.cbegin(), map.cend());
    
    // 插入元素
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    
    // 非空map迭代器
    EXPECT_NE(map.begin(), map.end());
    EXPECT_NE(map.cbegin(), map.cend());
    
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
    
    // 常量迭代器
    const auto& const_map = map;
    std::vector<int> const_keys;
    for (auto it = const_map.begin(); it != const_map.end(); ++it) {
        const_keys.push_back(it->first);
    }
    
    EXPECT_EQ(const_keys.size(), 3);
}

// swap测试
TEST_F(UnorderedMapTest, Swap) {
    unordered_map<int, std::string> map1;
    unordered_map<int, std::string> map2;
    
    map1.insert({1, "one"});
    map1.insert({2, "two"});
    map1.insert({3, "three"});
    
    map2.insert({4, "four"});
    map2.insert({5, "five"});
    
    // 交换
    map1.swap(map2);
    
    EXPECT_EQ(map1.size(), 2);
    EXPECT_EQ(map1.at(4), "four");
    EXPECT_EQ(map1.at(5), "five");
    
    EXPECT_EQ(map2.size(), 3);
    EXPECT_EQ(map2.at(1), "one");
    EXPECT_EQ(map2.at(2), "two");
    EXPECT_EQ(map2.at(3), "three");
}

// 哈希策略测试
TEST_F(UnorderedMapTest, HashPolicy) {
    unordered_map<int, std::string> map;
    
    // 获取哈希函数
    auto hash_func = map.hash_function();
    EXPECT_EQ(hash_func(1), hash_func(1));
    
    // 获取相等比较函数
    auto key_eq = map.key_eq();
    EXPECT_TRUE(key_eq(1, 1));
    EXPECT_FALSE(key_eq(1, 2));
}

// 字符串键测试
TEST_F(UnorderedMapTest, StringKeys) {
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

// 自定义哈希和比较函数测试
TEST_F(UnorderedMapTest, CustomHashAndEqual) {
    struct custom_hash {
        size_t operator()(int x) const { return x * 2; }
    };
    
    struct custom_equal {
        bool operator()(int a, int b) const { return a == b; }
    };
    
    unordered_map<int, std::string, custom_hash, custom_equal> map;
    
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.at(1), "one");
    EXPECT_EQ(map.at(2), "two");
    EXPECT_EQ(map.at(3), "three");
}

// 自动扩容测试
TEST_F(UnorderedMapTest, AutoRehash) {
    unordered_map<int, std::string> map;
    
    size_t initial_buckets = map.bucket_count();
    
    // 插入足够多的元素触发自动扩容
    for (int i = 0; i < 100; ++i) {
        map.insert({i, "value" + std::to_string(i)});
    }
    
    // 检查是否发生了扩容
    EXPECT_GT(map.bucket_count(), initial_buckets);
    EXPECT_EQ(map.size(), 100);
    
    // 验证所有元素仍然存在
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(map.count(i), 1);
    }
}

// 冲突处理测试
TEST_F(UnorderedMapTest, CollisionHandling) {
    // 使用一个会产生相同哈希值的简单哈希函数
    struct bad_hash {
        size_t operator()(int x) const { return 1; } // 所有键都映射到同一个桶
    };
    
    unordered_map<int, std::string, bad_hash> map;
    
    // 插入多个会产生冲突的元素
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    map.insert({4, "four"});
    map.insert({5, "five"});
    
    EXPECT_EQ(map.size(), 5);
    EXPECT_EQ(map.at(1), "one");
    EXPECT_EQ(map.at(2), "two");
    EXPECT_EQ(map.at(3), "three");
    EXPECT_EQ(map.at(4), "four");
    EXPECT_EQ(map.at(5), "five");
}

// 边界情况测试
TEST_F(UnorderedMapTest, EdgeCases) {
    unordered_map<int, std::string> map;
    
    // 删除空map的元素
    size_t count = map.erase(1);
    EXPECT_EQ(count, 0);
    
    // 删除end迭代器
    auto it = map.erase(map.end());
    EXPECT_EQ(it, map.end());
    
    // 清空空map
    EXPECT_NO_THROW(map.clear());
    
    // rehash空map
    EXPECT_NO_THROW(map.rehash(10));
    
    // reserve 0
    EXPECT_NO_THROW(map.reserve(0));
}

// 比较运算符测试
TEST_F(UnorderedMapTest, ComparisonOperators) {
    unordered_map<int, std::string> map1 = {{1, "one"}, {2, "two"}, {3, "three"}};
    unordered_map<int, std::string> map2 = {{1, "one"}, {2, "two"}, {3, "three"}};
    unordered_map<int, std::string> map3 = {{1, "one"}, {2, "two"}, {3, "THREE"}};
    unordered_map<int, std::string> map4 = {{1, "one"}, {2, "two"}};
    unordered_map<int, std::string> map5 = {{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};
    
    EXPECT_TRUE(map1 == map2);
    EXPECT_FALSE(map1 == map3);
    EXPECT_FALSE(map1 == map4);
    EXPECT_FALSE(map1 == map5);
    
    EXPECT_TRUE(map1 != map3);
    EXPECT_FALSE(map1 != map2);
}

// 内存管理测试
TEST_F(UnorderedMapTest, MemoryManagement) {
    unordered_map<int, std::unique_ptr<int>> map;
    
    // 插入unique_ptr
    map.insert({1, std::make_unique<int>(42)});
    map.insert({2, std::make_unique<int>(100)});
    
    EXPECT_EQ(map.size(), 2);
    
    // 验证unique_ptr内容
    auto it = map.find(1);
    EXPECT_NE(it, map.end());
    EXPECT_EQ(*it->second, 42);
    
    // 移动语义
    auto ptr = std::make_unique<int>(200);
    map.insert({3, std::move(ptr)});
    EXPECT_EQ(ptr.get(), nullptr);
    EXPECT_EQ(map.size(), 3);
}

// 分配器测试
TEST_F(UnorderedMapTest, AllocatorTest) {
    using TestAllocator = stl::allocator<std::pair<const int, std::string>>;
    unordered_map<int, std::string, stl::hash<int>, stl::equal_to<int>, TestAllocator> map;
    
    // 使用自定义分配器
    map.insert({1, "one"});
    map.insert({2, "two"});
    map.insert({3, "three"});
    
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.at(1), "one");
    EXPECT_EQ(map.at(2), "two");
    EXPECT_EQ(map.at(3), "three");
}

// 大规模数据测试
TEST_F(UnorderedMapTest, LargeScaleTest) {
    unordered_map<int, std::string> map;
    
    // 插入大量数据
    const int num_elements = 10000;
    for (int i = 0; i < num_elements; ++i) {
        map.insert({i, "value" + std::to_string(i)});
    }
    
    EXPECT_EQ(map.size(), num_elements);
    
    // 验证所有元素都存在
    for (int i = 0; i < num_elements; ++i) {
        EXPECT_EQ(map.count(i), 1);
    }
    
    // 验证不存在的元素
    EXPECT_EQ(map.count(-1), 0);
    EXPECT_EQ(map.count(num_elements), 0);
}

// 复杂类型测试
TEST_F(UnorderedMapTest, ComplexTypes) {
    unordered_map<std::string, std::vector<int>> map;
    
    // 插入复杂类型
    map.insert({"numbers", {1, 2, 3, 4, 5}});
    map.insert({"even", {2, 4, 6, 8}});
    map.insert({"odd", {1, 3, 5, 7, 9}});
    
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.at("numbers").size(), 5);
    EXPECT_EQ(map.at("even").size(), 4);
    EXPECT_EQ(map.at("odd").size(), 5);
    
    // 修改复杂类型
    map.at("numbers").push_back(6);
    EXPECT_EQ(map.at("numbers").size(), 6);
    
    // 访问复杂类型
    const auto& numbers = map.at("numbers");
    EXPECT_EQ(numbers[0], 1);
    EXPECT_EQ(numbers[5], 6);
}

// 重复插入测试
TEST_F(UnorderedMapTest, DuplicateInsertion) {
    unordered_map<int, std::string> map;
    
    // 多次插入相同键
    for (int i = 0; i < 10; ++i) {
        auto result = map.insert({42, "value"});
        if (i == 0) {
            EXPECT_TRUE(result.second);
        } else {
            EXPECT_FALSE(result.second);
        }
    }
    
    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map.at(42), "value");
}

// 混合操作测试
TEST_F(UnorderedMapTest, MixedOperations) {
    unordered_map<int, std::string> map;
    
    // 混合插入、删除、查找操作
    for (int i = 0; i < 100; ++i) {
        map.insert({i, "value" + std::to_string(i)});
        EXPECT_EQ(map.count(i), 1);
        
        if (i % 2 == 0) {
            map.erase(i);
            EXPECT_EQ(map.count(i), 0);
        }
    }
    
    EXPECT_EQ(map.size(), 50);
    
    // 验证奇数存在，偶数不存在
    for (int i = 0; i < 100; ++i) {
        if (i % 2 == 1) {
            EXPECT_EQ(map.count(i), 1);
        } else {
            EXPECT_EQ(map.count(i), 0);
        }
    }
}