#include <gtest/gtest.h>
#include "stl/container/map.hpp"
#include <string>

using namespace stl;

// Test map constructor and basic operations
TEST(MapTest, ConstructorAndBasicOperations) {
    map<int, std::string> m;
    
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0);
    
    m.insert({1, "one"});
    m.insert({2, "two"});
    m.insert({3, "three"});
    
    EXPECT_FALSE(m.empty());
    EXPECT_EQ(m.size(), 3);
    
    EXPECT_EQ(m[1], "one");
    EXPECT_EQ(m[2], "two");
    EXPECT_EQ(m[3], "three");
}

// Test map operator[]
TEST(MapTest, OperatorBracket) {
    map<int, std::string> m;
    
    // Insert using operator[]
    m[1] = "one";
    m[2] = "two";
    
    EXPECT_EQ(m.size(), 2);
    EXPECT_EQ(m[1], "one");
    EXPECT_EQ(m[2], "two");
    
    // Modify existing value
    m[1] = "modified";
    EXPECT_EQ(m[1], "modified");
    EXPECT_EQ(m.size(), 2);
    
    // Access non-existent key (should insert default value)
    std::string& val = m[3];
    EXPECT_EQ(m.size(), 3);
    EXPECT_EQ(val, "");
    EXPECT_EQ(m[3], "");
}

// Test map insert operations
TEST(MapTest, InsertOperations) {
    map<int, std::string> m;
    
    // Insert single element
    auto result1 = m.insert({1, "one"});
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(result1.first->first, 1);
    EXPECT_EQ(result1.first->second, "one");
    
    // Insert duplicate (should fail)
    auto result2 = m.insert({1, "duplicate"});
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(result2.first->first, 1);
    EXPECT_EQ(result2.first->second, "one");
    
    // Insert with hint
    value_type pair(2, "two");
    auto hint = m.insert(m.begin(), pair);
    EXPECT_EQ(hint->first, 2);
    EXPECT_EQ(hint->second, "two");
    
    // Insert range
    std::vector<std::pair<int, std::string>> pairs = {{3, "three"}, {4, "four"}};
    m.insert(pairs.begin(), pairs.end());
    EXPECT_EQ(m.size(), 4);
    
    // Insert initializer list
    m.insert({{5, "five"}, {6, "six"}});
    EXPECT_EQ(m.size(), 6);
}

// Test map emplace operations
TEST(MapTest, EmplaceOperations) {
    map<int, std::string> m;
    
    // Emplace
    auto result1 = m.emplace(1, "one");
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(result1.first->first, 1);
    EXPECT_EQ(result1.first->second, "one");
    
    // Emplace duplicate (should fail)
    auto result2 = m.emplace(1, "duplicate");
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(result2.first->first, 1);
    EXPECT_EQ(result2.first->second, "one");
    
    // Emplace with hint
    auto hint = m.emplace_hint(m.begin(), 2, "two");
    EXPECT_EQ(hint->first, 2);
    EXPECT_EQ(hint->second, "two");
}

// Test map try_emplace
TEST(MapTest, TryEmplace) {
    map<int, std::string> m;
    
    // Try emplace new key
    auto result1 = m.try_emplace(1, "one");
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(result1.first->first, 1);
    EXPECT_EQ(result1.first->second, "one");
    
    // Try emplace existing key (should fail)
    auto result2 = m.try_emplace(1, "duplicate");
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(result2.first->first, 1);
    EXPECT_EQ(result2.first->second, "one");
    
    // Try emplace with rvalue key
    auto result3 = m.try_emplace(2, "two");
    EXPECT_TRUE(result3.second);
    EXPECT_EQ(result3.first->first, 2);
    EXPECT_EQ(result3.first->second, "two");
}

// Test map insert_or_assign
TEST(MapTest, InsertOrAssign) {
    map<int, std::string> m;
    
    // Insert or assign new key
    auto result1 = m.insert_or_assign(1, "one");
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(result1.first->first, 1);
    EXPECT_EQ(result1.first->second, "one");
    
    // Insert or assign existing key (should modify)
    auto result2 = m.insert_or_assign(1, "modified");
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(result2.first->first, 1);
    EXPECT_EQ(result2.first->second, "modified");
    
    // Insert or assign with rvalue key
    auto result3 = m.insert_or_assign(2, "two");
    EXPECT_TRUE(result3.second);
    EXPECT_EQ(result3.first->first, 2);
    EXPECT_EQ(result3.first->second, "two");
}

// Test map at() method
TEST(MapTest, AtMethod) {
    map<int, std::string> m = {{1, "one"}, {2, "two"}};
    
    EXPECT_EQ(m.at(1), "one");
    EXPECT_EQ(m.at(2), "two");
    
    // Test const version
    const auto& cm = m;
    EXPECT_EQ(cm.at(1), "one");
    EXPECT_EQ(cm.at(2), "two");
    
    // Test out_of_range exception
    EXPECT_THROW(m.at(3), std::out_of_range);
    EXPECT_THROW(cm.at(3), std::out_of_range);
}

// Test map erase operations
TEST(MapTest, EraseOperations) {
    map<int, std::string> m = {{1, "one"}, {2, "two"}, {3, "three"}};
    
    // Erase by key
    EXPECT_EQ(m.erase(2), 1);
    EXPECT_EQ(m.size(), 2);
    EXPECT_EQ(m.erase(99), 0);  // Non-existent key
    EXPECT_EQ(m.size(), 2);
    
    // Erase by iterator
    auto it = m.find(1);
    auto next = m.erase(it);
    EXPECT_EQ(m.size(), 1);
    EXPECT_NE(next, m.end());
    EXPECT_EQ(next->first, 3);
    
    // Erase by const_iterator
    auto cit = m.find(3);
    auto cnext = m.erase(cit);
    EXPECT_EQ(m.size(), 0);
    EXPECT_EQ(cnext, m.end());
    
    // Erase range
    m = {{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};
    auto first = m.find(2);
    auto last = m.find(4);
    m.erase(first, last);
    EXPECT_EQ(m.size(), 2);
    EXPECT_TRUE(m.find(2) == m.end());
    EXPECT_TRUE(m.find(3) == m.end());
    EXPECT_TRUE(m.find(1) != m.end());
    EXPECT_TRUE(m.find(4) != m.end());
}

// Test map find operations
TEST(MapTest, FindOperations) {
    map<int, std::string> m = {{1, "one"}, {2, "two"}, {3, "three"}};
    
    // Find existing key
    auto it = m.find(2);
    EXPECT_NE(it, m.end());
    EXPECT_EQ(it->first, 2);
    EXPECT_EQ(it->second, "two");
    
    // Find non-existent key
    auto it2 = m.find(99);
    EXPECT_EQ(it2, m.end());
    
    // Count
    EXPECT_EQ(m.count(2), 1);
    EXPECT_EQ(m.count(99), 0);
    
    // Contains
    EXPECT_TRUE(m.contains(2));
    EXPECT_FALSE(m.contains(99));
}

// Test map range operations
TEST(MapTest, RangeOperations) {
    map<int, std::string> m = {{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};
    
    // Lower bound
    auto lb = m.lower_bound(2);
    EXPECT_NE(lb, m.end());
    EXPECT_EQ(lb->first, 2);
    
    // Upper bound
    auto ub = m.upper_bound(2);
    EXPECT_NE(ub, m.end());
    EXPECT_EQ(ub->first, 3);
    
    // Equal range
    auto range = m.equal_range(2);
    EXPECT_NE(range.first, m.end());
    EXPECT_NE(range.second, m.end());
    EXPECT_EQ(range.first->first, 2);
    EXPECT_EQ(range.second->first, 3);
    
    // Test with non-existent key
    auto range2 = m.equal_range(99);
    EXPECT_EQ(range2.first, m.end());
    EXPECT_EQ(range2.second, m.end());
}

// Test map iterator operations
TEST(MapTest, IteratorOperations) {
    map<int, std::string> m = {{1, "one"}, {2, "two"}, {3, "three"}};
    
    // Forward iteration
    std::vector<int> keys;
    for (const auto& kv : m) {
        keys.push_back(kv.first);
    }
    std::vector<int> expected = {1, 2, 3};
    EXPECT_EQ(keys, expected);
    
    // Reverse iteration
    std::vector<int> reverse_keys;
    for (auto it = m.rbegin(); it != m.rend(); ++it) {
        reverse_keys.push_back(it->first);
    }
    std::vector<int> expected_reverse = {3, 2, 1};
    EXPECT_EQ(reverse_keys, expected_reverse);
    
    // Const iteration
    const auto& cm = m;
    std::vector<int> const_keys;
    for (const auto& kv : cm) {
        const_keys.push_back(kv.first);
    }
    EXPECT_EQ(const_keys, expected);
}

// Test map comparison operators
TEST(MapTest, ComparisonOperators) {
    map<int, std::string> m1 = {{1, "one"}, {2, "two"}, {3, "three"}};
    map<int, std::string> m2 = {{1, "one"}, {2, "two"}, {3, "three"}};
    map<int, std::string> m3 = {{1, "one"}, {2, "modified"}, {3, "three"}};
    map<int, std::string> m4 = {{1, "one"}, {2, "two"}};
    map<int, std::string> m5 = {{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}};
    
    EXPECT_EQ(m1, m2);
    EXPECT_NE(m1, m3);
    EXPECT_NE(m1, m4);
    EXPECT_NE(m1, m5);
    
    EXPECT_LT(m4, m1);
    EXPECT_LE(m4, m1);
    EXPECT_GT(m1, m4);
    EXPECT_GE(m1, m4);
}

// Test map swap operation
TEST(MapTest, SwapOperation) {
    map<int, std::string> m1 = {{1, "one"}, {2, "two"}};
    map<int, std::string> m2 = {{3, "three"}, {4, "four"}};
    
    swap(m1, m2);
    
    EXPECT_EQ(m1.size(), 2);
    EXPECT_EQ(m1.at(3), "three");
    EXPECT_EQ(m1.at(4), "four");
    
    EXPECT_EQ(m2.size(), 2);
    EXPECT_EQ(m2.at(1), "one");
    EXPECT_EQ(m2.at(2), "two");
}

// Test map with custom comparator
TEST(MapTest, CustomComparator) {
    map<int, std::string, std::greater<int>> m;
    
    m.insert({3, "three"});
    m.insert({1, "one"});
    m.insert({2, "two"});
    
    // Should be in descending order
    auto it = m.begin();
    EXPECT_EQ(it->first, 3);
    ++it;
    EXPECT_EQ(it->first, 2);
    ++it;
    EXPECT_EQ(it->first, 1);
}

// Test map move semantics
TEST(MapTest, MoveSemantics) {
    map<int, std::string> m1 = {{1, "one"}, {2, "two"}};
    auto size = m1.size();
    
    map<int, std::string> m2 = std::move(m1);
    EXPECT_EQ(m2.size(), size);
    EXPECT_TRUE(m1.empty());
    
    map<int, std::string> m3;
    m3 = std::move(m2);
    EXPECT_EQ(m3.size(), size);
    EXPECT_TRUE(m2.empty());
}

// Test map copy semantics
TEST(MapTest, CopySemantics) {
    map<int, std::string> m1 = {{1, "one"}, {2, "two"}};
    
    map<int, std::string> m2 = m1;
    EXPECT_EQ(m2.size(), m1.size());
    EXPECT_EQ(m2.at(1), "one");
    EXPECT_EQ(m2.at(2), "two");
    
    map<int, std::string> m3;
    m3 = m1;
    EXPECT_EQ(m3.size(), m1.size());
    EXPECT_EQ(m3.at(1), "one");
    EXPECT_EQ(m3.at(2), "two");
}

// Test map initializer list constructor
TEST(MapTest, InitializerListConstructor) {
    map<int, std::string> m = {{1, "one"}, {2, "two"}, {3, "three"}};
    
    EXPECT_EQ(m.size(), 3);
    EXPECT_EQ(m.at(1), "one");
    EXPECT_EQ(m.at(2), "two");
    EXPECT_EQ(m.at(3), "three");
}

// Test map clear operation
TEST(MapTest, ClearOperation) {
    map<int, std::string> m = {{1, "one"}, {2, "two"}, {3, "three"}};
    
    EXPECT_FALSE(m.empty());
    EXPECT_EQ(m.size(), 3);
    
    m.clear();
    
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0);
}

// Test map max_size
TEST(MapTest, MaxSize) {
    map<int, std::string> m;
    EXPECT_GT(m.max_size(), 0);
}

// Test map key_comp and value_comp
TEST(MapTest, Comparators) {
    map<int, std::string> m;
    
    auto key_comp = m.key_comp();
    EXPECT_TRUE(key_comp(1, 2));
    EXPECT_FALSE(key_comp(2, 1));
    
    auto value_comp = m.value_comp();
    EXPECT_TRUE(value_comp({1, "a"}, {2, "b"}));
    EXPECT_FALSE(value_comp({2, "b"}, {1, "a"}));
}