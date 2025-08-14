#include <gtest/gtest.h>
#include "stl/container/set.hpp"
#include <string>
#include <set>

TEST(SetTest, BasicOperations) {
    stl::set<int> s;
    
    // 测试空集合
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
    
    // 测试插入
    auto result1 = s.insert(10);
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(*result1.first, 10);
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.size(), 1);
    
    // 测试重复插入
    auto result2 = s.insert(10);
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(s.size(), 1);
    
    // 测试查找
    auto it = s.find(10);
    EXPECT_NE(it, s.end());
    EXPECT_EQ(*it, 10);
    
    auto not_found = s.find(20);
    EXPECT_EQ(not_found, s.end());
    
    // 测试计数
    EXPECT_EQ(s.count(10), 1);
    EXPECT_EQ(s.count(20), 0);
    
    // 测试删除
    size_t erased = s.erase(10);
    EXPECT_EQ(erased, 1);
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
    
    erased = s.erase(10);
    EXPECT_EQ(erased, 0);
}

TEST(SetTest, MultipleElements) {
    stl::set<int> s;
    
    // 插入多个元素
    s.insert(30);
    s.insert(10);
    s.insert(20);
    s.insert(40);
    
    EXPECT_EQ(s.size(), 4);
    
    // 验证元素按顺序排列
    auto it = s.begin();
    EXPECT_EQ(*it++, 10);
    EXPECT_EQ(*it++, 20);
    EXPECT_EQ(*it++, 30);
    EXPECT_EQ(*it++, 40);
    EXPECT_EQ(it, s.end());
    
    // 测试删除
    size_t erased = s.erase(20);
    EXPECT_EQ(erased, 1);
    
    EXPECT_EQ(s.size(), 3);
    it = s.begin();
    EXPECT_EQ(*it++, 10);
    EXPECT_EQ(*it++, 30);
    EXPECT_EQ(*it++, 40);
}

TEST(SetTest, IteratorOperations) {
    stl::set<std::string> s;
    
    s.insert("world");
    s.insert("hello");
    s.insert("stl");
    
    // 测试正向迭代器
    std::vector<std::string> expected = {"hello", "stl", "world"};
    std::vector<std::string> actual;
    for (const auto& item : s) {
        actual.push_back(item);
    }
    EXPECT_EQ(actual, expected);
    
    // 测试反向迭代器 (暂时跳过)
    /*
    std::vector<std::string> reversed;
    for (auto it = s.rbegin(); it != s.rend(); ++it) {
        reversed.push_back(*it);
    }
    std::reverse(expected.begin(), expected.end());
    EXPECT_EQ(reversed, expected);
    */
}

TEST(SetTest, RangeOperations) {
    stl::set<int> s;
    
    for (int i = 1; i <= 10; ++i) {
        s.insert(i);
    }
    
    // 测试 lower_bound
    auto lb = s.lower_bound(5);
    EXPECT_EQ(*lb, 5);
    
    lb = s.lower_bound(15);
    EXPECT_EQ(lb, s.end());
    
    // 测试 upper_bound
    auto ub = s.upper_bound(5);
    EXPECT_EQ(*ub, 6);
    
    ub = s.upper_bound(10);
    EXPECT_EQ(ub, s.end());
    
    // 测试 equal_range
    auto range = s.equal_range(5);
    EXPECT_EQ(*range.first, 5);
    EXPECT_EQ(*range.second, 6);
}

TEST(SetTest, InitializerList) {
    stl::set<int> s = {5, 1, 3, 2, 4};
    
    EXPECT_EQ(s.size(), 5);
    
    // 验证排序
    auto it = s.begin();
    EXPECT_EQ(*it++, 1);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 3);
    EXPECT_EQ(*it++, 4);
    EXPECT_EQ(*it++, 5);
}

TEST(SetTest, CopyAndMove) {
    stl::set<int> original = {1, 2, 3};
    
    // 测试拷贝构造
    stl::set<int> copy(original);
    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(copy, original);
    
    // 测试赋值运算符
    stl::set<int> assigned;
    assigned = original;
    EXPECT_EQ(assigned.size(), 3);
    EXPECT_EQ(assigned, original);
    
    // 测试移动构造
    stl::set<int> moved(std::move(original));
    EXPECT_EQ(moved.size(), 3);
    EXPECT_TRUE(original.empty());
}

TEST(SetTest, ComparisonOperators) {
    stl::set<int> s1 = {1, 2, 3};
    stl::set<int> s2 = {1, 2, 3};
    stl::set<int> s3 = {1, 2, 4};
    stl::set<int> s4 = {1, 2};
    
    EXPECT_EQ(s1, s2);
    EXPECT_NE(s1, s3);
    EXPECT_LT(s4, s1);
    EXPECT_GT(s1, s4);
    EXPECT_LE(s1, s2);
    EXPECT_GE(s1, s4);
}

TEST(SetTest, Emplace) {
    stl::set<std::pair<int, std::string>> s;
    
    // 测试 emplace
    auto result = s.emplace(1, "one");
    EXPECT_TRUE(result.second);
    EXPECT_EQ(result.first->first, 1);
    EXPECT_EQ(result.first->second, "one");
    
    // 测试重复 emplace
    result = s.emplace(1, "one");
    EXPECT_FALSE(result.second);
}

TEST(SetTest, LargeDataset) {
    stl::set<int> s;
    
    // 插入大量数据
    for (int i = 0; i < 1000; ++i) {
        s.insert(i);
    }
    
    EXPECT_EQ(s.size(), 1000);
    
    // 验证查找性能
    for (int i = 0; i < 1000; ++i) {
        auto it = s.find(i);
        EXPECT_NE(it, s.end());
        EXPECT_EQ(*it, i);
    }
    
    // 验证不存在元素
    EXPECT_EQ(s.find(1000), s.end());
    
    // 清空
    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
}

TEST(SetTest, StringKeys) {
    stl::set<std::string> s;
    
    s.insert("banana");
    s.insert("apple");
    s.insert("cherry");
    
    EXPECT_EQ(s.size(), 3);
    
    // 验证字典序
    auto it = s.begin();
    EXPECT_EQ(*it++, "apple");
    EXPECT_EQ(*it++, "banana");
    EXPECT_EQ(*it++, "cherry");
}

TEST(SetTest, CustomComparator) {
    // 使用 greater 作为比较器
    stl::set<int, stl::greater<int>> s;
    
    s.insert(10);
    s.insert(30);
    s.insert(20);
    
    // 验证降序排列
    auto it = s.begin();
    EXPECT_EQ(*it++, 30);
    EXPECT_EQ(*it++, 20);
    EXPECT_EQ(*it++, 10);
}