#include <gtest/gtest.h>
#include "stl/container/multiset.hpp"
#include <string>
#include <vector>

TEST(MultiSetTest, BasicOperations) {
    stl::multiset<int> ms;
    
    // 测试空集合
    EXPECT_TRUE(ms.empty());
    EXPECT_EQ(ms.size(), 0);
    
    // 测试插入
    auto it1 = ms.insert(10);
    EXPECT_EQ(*it1, 10);
    EXPECT_FALSE(ms.empty());
    EXPECT_EQ(ms.size(), 1);
    
    // 测试重复插入
    auto it2 = ms.insert(10);
    EXPECT_EQ(*it2, 10);
    EXPECT_EQ(ms.size(), 2);
    
    // 测试查找
    auto found = ms.find(10);
    EXPECT_NE(found, ms.end());
    EXPECT_EQ(*found, 10);
    
    auto not_found = ms.find(20);
    EXPECT_EQ(not_found, ms.end());
    
    // 测试计数
    EXPECT_EQ(ms.count(10), 2);
    EXPECT_EQ(ms.count(20), 0);
    
    // 测试删除
    size_t erased = ms.erase(10);
    EXPECT_EQ(erased, 2);
    EXPECT_TRUE(ms.empty());
    EXPECT_EQ(ms.size(), 0);
    
    erased = ms.erase(10);
    EXPECT_EQ(erased, 0);
}

TEST(MultiSetTest, MultipleElements) {
    stl::multiset<int> ms;
    
    // 插入多个元素，包含重复
    ms.insert(30);
    ms.insert(10);
    ms.insert(20);
    ms.insert(10);
    ms.insert(30);
    ms.insert(40);
    
    EXPECT_EQ(ms.size(), 6);
    
    // 验证元素按顺序排列
    std::vector<int> expected = {10, 10, 20, 30, 30, 40};
    std::vector<int> actual;
    for (const auto& item : ms) {
        actual.push_back(item);
    }
    EXPECT_EQ(actual, expected);
    
    // 测试删除特定键
    size_t erased = ms.erase(30);
    EXPECT_EQ(erased, 2);
    EXPECT_EQ(ms.size(), 4);
    
    // 验证剩余元素
    expected = {10, 10, 20, 40};
    actual.clear();
    for (const auto& item : ms) {
        actual.push_back(item);
    }
    EXPECT_EQ(actual, expected);
}

TEST(MultiSetTest, IteratorOperations) {
    stl::multiset<std::string> ms;
    
    ms.insert("world");
    ms.insert("hello");
    ms.insert("hello");
    ms.insert("stl");
    
    // 测试正向迭代器
    std::vector<std::string> expected = {"hello", "hello", "stl", "world"};
    std::vector<std::string> actual;
    for (const auto& item : ms) {
        actual.push_back(item);
    }
    EXPECT_EQ(actual, expected);
    
    // 测试范围删除
    auto range = ms.equal_range("hello");
    size_t count = 0;
    while (range.first != range.second) {
        range.first = ms.erase(range.first);
        count++;
    }
    EXPECT_EQ(count, 2);
    EXPECT_EQ(ms.size(), 2);
}

TEST(MultiSetTest, RangeOperations) {
    stl::multiset<int> ms;
    
    for (int i = 1; i <= 5; ++i) {
        ms.insert(i);
        ms.insert(i); // 每个元素插入两次
    }
    
    EXPECT_EQ(ms.size(), 10);
    
    // 测试 lower_bound
    auto lb = ms.lower_bound(3);
    EXPECT_EQ(*lb, 3);
    
    // 测试 upper_bound
    auto ub = ms.upper_bound(3);
    EXPECT_EQ(*ub, 4);
    
    // 测试 equal_range
    auto range = ms.equal_range(3);
    std::vector<int> values;
    for (auto it = range.first; it != range.second; ++it) {
        values.push_back(*it);
    }
    std::vector<int> expected = {3, 3};
    EXPECT_EQ(values, expected);
}

TEST(MultiSetTest, InitializerList) {
    stl::multiset<int> ms = {5, 1, 3, 2, 4, 1, 3};
    
    EXPECT_EQ(ms.size(), 7);
    
    // 验证排序和重复
    std::vector<int> expected = {1, 1, 2, 3, 3, 4, 5};
    std::vector<int> actual;
    for (const auto& item : ms) {
        actual.push_back(item);
    }
    EXPECT_EQ(actual, expected);
}

TEST(MultiSetTest, CopyAndMove) {
    stl::multiset<int> original = {1, 1, 2, 3};
    
    // 测试拷贝构造
    stl::multiset<int> copy(original);
    EXPECT_EQ(copy.size(), 4);
    EXPECT_EQ(copy, original);
    
    // 测试赋值运算符
    stl::multiset<int> assigned;
    assigned = original;
    EXPECT_EQ(assigned.size(), 4);
    EXPECT_EQ(assigned, original);
    
    // 测试移动构造
    stl::multiset<int> moved(std::move(original));
    EXPECT_EQ(moved.size(), 4);
    EXPECT_TRUE(original.empty());
}

TEST(MultiSetTest, ComparisonOperators) {
    stl::multiset<int> ms1 = {1, 1, 2, 3};
    stl::multiset<int> ms2 = {1, 1, 2, 3};
    stl::multiset<int> ms3 = {1, 2, 2, 3};
    stl::multiset<int> ms4 = {1, 1, 2};
    
    EXPECT_EQ(ms1, ms2);
    EXPECT_NE(ms1, ms3);
    EXPECT_LT(ms4, ms1);
    EXPECT_GT(ms1, ms4);
    EXPECT_LE(ms1, ms2);
    EXPECT_GE(ms1, ms4);
}

TEST(MultiSetTest, Emplace) {
    stl::multiset<std::pair<int, std::string>> ms;
    
    // 测试 emplace
    auto it = ms.emplace(1, "one");
    EXPECT_EQ(it->first, 1);
    EXPECT_EQ(it->second, "one");
    
    // 测试重复 emplace
    it = ms.emplace(1, "one");
    EXPECT_EQ(it->first, 1);
    EXPECT_EQ(it->second, "one");
}

TEST(MultiSetTest, LargeDataset) {
    stl::multiset<int> ms;
    
    // 插入大量数据，包含重复
    for (int i = 0; i < 1000; ++i) {
        ms.insert(i % 100); // 0-99，每个数字出现10次
    }
    
    EXPECT_EQ(ms.size(), 1000);
    
    // 验证每个键的计数
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(ms.count(i), 10);
    }
    
    // 验证不存在元素
    EXPECT_EQ(ms.count(100), 0);
    
    // 清空
    ms.clear();
    EXPECT_TRUE(ms.empty());
    EXPECT_EQ(ms.size(), 0);
}

TEST(MultiSetTest, StringKeys) {
    stl::multiset<std::string> ms;
    
    ms.insert("banana");
    ms.insert("apple");
    ms.insert("apple");
    ms.insert("cherry");
    ms.insert("banana");
    
    EXPECT_EQ(ms.size(), 5);
    
    // 验证字典序和重复
    std::vector<std::string> expected = {"apple", "apple", "banana", "banana", "cherry"};
    std::vector<std::string> actual;
    for (const auto& item : ms) {
        actual.push_back(item);
    }
    EXPECT_EQ(actual, expected);
}

TEST(MultiSetTest, CustomComparator) {
    // 使用 greater 作为比较器
    stl::multiset<int, stl::greater<int>> ms;
    
    ms.insert(10);
    ms.insert(30);
    ms.insert(20);
    ms.insert(20);
    
    // 验证降序排列
    std::vector<int> expected = {30, 20, 20, 10};
    std::vector<int> actual;
    for (const auto& item : ms) {
        actual.push_back(item);
    }
    EXPECT_EQ(actual, expected);
}

TEST(MultiSetTest, EraseRange) {
    stl::multiset<int> ms = {1, 2, 2, 3, 3, 3, 4, 5};
    
    // 删除所有值为3的元素
    auto range = ms.equal_range(3);
    auto first = range.first;
    auto last = range.second;
    size_t count = 0;
    while (first != last) {
        first = ms.erase(first);
        count++;
    }
    EXPECT_EQ(count, 3);
    
    // 验证剩余元素
    std::vector<int> expected = {1, 2, 2, 4, 5};
    std::vector<int> actual;
    for (const auto& item : ms) {
        actual.push_back(item);
    }
    EXPECT_EQ(actual, expected);
}