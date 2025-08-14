#include <gtest/gtest.h>
#include "stl/container/multimap.hpp"
#include <string>

using namespace stl;

// Test multimap constructor and basic operations
TEST(MultimapTest, ConstructorAndBasicOperations) {
    multimap<int, std::string> m;
    
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0);
    
    m.insert({1, "one"});
    m.insert({2, "two"});
    m.insert({3, "three"});
    
    EXPECT_FALSE(m.empty());
    EXPECT_EQ(m.size(), 3);
}

// Test multimap duplicate key insertion
TEST(MultimapTest, DuplicateKeyInsertion) {
    multimap<int, std::string> m;
    
    // Insert multiple values with the same key
    m.insert({1, "first"});
    m.insert({1, "second"});
    m.insert({1, "third"});
    m.insert({2, "two"});
    m.insert({1, "fourth"});
    
    EXPECT_EQ(m.size(), 5);
    EXPECT_EQ(m.count(1), 4);
    EXPECT_EQ(m.count(2), 1);
    EXPECT_EQ(m.count(3), 0);
}

// Test multimap insert operations
TEST(MultimapTest, InsertOperations) {
    multimap<int, std::string> m;
    
    // Insert single element
    auto it1 = m.insert({1, "one"});
    EXPECT_EQ(it1->first, 1);
    EXPECT_EQ(it1->second, "one");
    
    // Insert duplicate (should succeed for multimap)
    auto it2 = m.insert({1, "duplicate"});
    EXPECT_EQ(it2->first, 1);
    EXPECT_EQ(it2->second, "duplicate");
    
    // Insert with hint
    multimap<int, std::string>::value_type pair(2, "two");
    auto hint = m.insert(m.cbegin(), pair);
    EXPECT_EQ(hint->first, 2);
    EXPECT_EQ(hint->second, "two");
    
    // Insert range
    std::vector<std::pair<int, std::string>> pairs = {{3, "three"}, {4, "four"}};
    m.insert(pairs.begin(), pairs.end());
    EXPECT_EQ(m.size(), 5);
    
    // Insert initializer list
    m.insert({{5, "five"}, {6, "six"}});
    EXPECT_EQ(m.size(), 7);
}

// Test multimap emplace operations
TEST(MultimapTest, EmplaceOperations) {
    multimap<int, std::string> m;
    
    // Emplace
    auto it1 = m.emplace(1, "one");
    EXPECT_EQ(it1->first, 1);
    EXPECT_EQ(it1->second, "one");
    
    // Emplace duplicate (should succeed for multimap)
    auto it2 = m.emplace(1, "duplicate");
    EXPECT_EQ(it2->first, 1);
    EXPECT_EQ(it2->second, "duplicate");
    
    // Emplace with hint
    auto hint = m.emplace_hint(m.cbegin(), 2, "two");
    EXPECT_EQ(hint->first, 2);
    EXPECT_EQ(hint->second, "two");
}

// Test multimap erase operations
TEST(MultimapTest, EraseOperations) {
    multimap<int, std::string> m = {{1, "one"}, {1, "uno"}, {2, "two"}, {3, "three"}};
    
    // Test erasing by key
    EXPECT_EQ(m.erase(1), 2); // Should erase both entries with key 1
    EXPECT_EQ(m.size(), 2);
    
    // Test erasing non-existent key
    EXPECT_EQ(m.erase(99), 0);
    EXPECT_EQ(m.size(), 2);
    
    // Test erasing remaining elements
    EXPECT_EQ(m.erase(2), 1);
    EXPECT_EQ(m.erase(3), 1);
    EXPECT_EQ(m.size(), 0);
}

// Test multimap find operations
TEST(MultimapTest, FindOperations) {
    multimap<int, std::string> m = {{1, "one"}, {1, "uno"}, {2, "two"}, {3, "three"}};
    
    // Find existing key
    auto it = m.find(1);
    EXPECT_NE(it, m.end());
    EXPECT_EQ(it->first, 1);
    
    // Find non-existent key
    auto it2 = m.find(99);
    EXPECT_EQ(it2, m.end());
    
    // Count
    EXPECT_EQ(m.count(1), 2);
    EXPECT_EQ(m.count(2), 1);
    EXPECT_EQ(m.count(99), 0);
    
    // Contains
    EXPECT_TRUE(m.contains(1));
    EXPECT_TRUE(m.contains(2));
    EXPECT_FALSE(m.contains(99));
}

// Test multimap range operations
TEST(MultimapTest, RangeOperations) {
    multimap<int, std::string> m = {{1, "one"}, {1, "uno"}, {2, "two"}, {3, "three"}, {1, "eins"}};
    
    // Lower bound
    auto lb = m.lower_bound(1);
    EXPECT_NE(lb, m.end());
    EXPECT_EQ(lb->first, 1);
    
    // Upper bound
    auto ub = m.upper_bound(1);
    EXPECT_NE(ub, m.end());
    EXPECT_EQ(ub->first, 2);
    
    // Equal range
    auto range = m.equal_range(1);
    EXPECT_NE(range.first, m.end());
    EXPECT_NE(range.second, m.end());
    
    // Count elements in range
    int count = 0;
    for (auto it = range.first; it != range.second; ++it) {
        count++;
        EXPECT_EQ(it->first, 1);
    }
    EXPECT_EQ(count, 3);
    
    // Test with non-existent key
    auto range2 = m.equal_range(99);
    EXPECT_EQ(range2.first, m.end());
    EXPECT_EQ(range2.second, m.end());
}

// Test multimap iterator operations
TEST(MultimapTest, IteratorOperations) {
    multimap<int, std::string> m = {{1, "one"}, {2, "two"}, {3, "three"}, {1, "uno"}};
    
    // Forward iteration
    std::vector<std::pair<int, std::string>> elements;
    for (const auto& kv : m) {
        elements.push_back(kv);
    }
    EXPECT_EQ(elements.size(), 4);
    
    // Should be sorted by key
    EXPECT_EQ(elements[0].first, 1);
    EXPECT_EQ(elements[1].first, 1);
    EXPECT_EQ(elements[2].first, 2);
    EXPECT_EQ(elements[3].first, 3);
    
    // Reverse iteration
    std::vector<std::pair<int, std::string>> reverse_elements;
    for (auto it = m.rbegin(); it != m.rend(); ++it) {
        reverse_elements.push_back(*it);
    }
    EXPECT_EQ(reverse_elements.size(), 4);
    
    // Should be in reverse order
    EXPECT_EQ(reverse_elements[0].first, 3);
    EXPECT_EQ(reverse_elements[1].first, 2);
    EXPECT_EQ(reverse_elements[2].first, 1);
    EXPECT_EQ(reverse_elements[3].first, 1);
    
    // Const iteration
    const auto& cm = m;
    std::vector<std::pair<int, std::string>> const_elements;
    for (const auto& kv : cm) {
        const_elements.push_back(kv);
    }
    EXPECT_EQ(const_elements, elements);
}

// Test multimap comparison operators
TEST(MultimapTest, ComparisonOperators) {
    multimap<int, std::string> m1 = {{1, "one"}, {1, "uno"}, {2, "two"}};
    multimap<int, std::string> m2 = {{1, "one"}, {1, "uno"}, {2, "two"}};
    multimap<int, std::string> m3 = {{1, "one"}, {1, "different"}, {2, "two"}};
    multimap<int, std::string> m4 = {{1, "one"}, {2, "two"}};
    multimap<int, std::string> m5 = {{1, "one"}, {1, "uno"}, {2, "two"}, {3, "three"}};
    
    EXPECT_EQ(m1, m2);
    EXPECT_NE(m1, m3);
    EXPECT_NE(m1, m4);
    EXPECT_NE(m1, m5);
    
    EXPECT_LT(m4, m1);
    EXPECT_LE(m4, m1);
    EXPECT_GT(m1, m4);
    EXPECT_GE(m1, m4);
}

// Test multimap swap operation
TEST(MultimapTest, SwapOperation) {
    multimap<int, std::string> m1 = {{1, "one"}, {1, "uno"}};
    multimap<int, std::string> m2 = {{2, "two"}, {3, "three"}};
    
    swap(m1, m2);
    
    EXPECT_EQ(m1.size(), 2);
    EXPECT_EQ(m1.count(2), 1);
    EXPECT_EQ(m1.count(3), 1);
    
    EXPECT_EQ(m2.size(), 2);
    EXPECT_EQ(m2.count(1), 2);
}

// Test multimap with custom comparator
TEST(MultimapTest, CustomComparator) {
    multimap<int, std::string, std::greater<int>> m;
    
    m.insert({3, "three"});
    m.insert({1, "one"});
    m.insert({2, "two"});
    m.insert({1, "uno"});
    
    // Should be in descending order
    auto it = m.begin();
    EXPECT_EQ(it->first, 3);
    ++it;
    EXPECT_EQ(it->first, 2);
    ++it;
    EXPECT_EQ(it->first, 1);
    ++it;
    EXPECT_EQ(it->first, 1);
}

// Test multimap move semantics
TEST(MultimapTest, MoveSemantics) {
    multimap<int, std::string> m1 = {{1, "one"}, {1, "uno"}};
    auto size = m1.size();
    
    multimap<int, std::string> m2 = std::move(m1);
    EXPECT_EQ(m2.size(), size);
    EXPECT_TRUE(m1.empty());
    
    multimap<int, std::string> m3;
    m3 = std::move(m2);
    EXPECT_EQ(m3.size(), size);
    EXPECT_TRUE(m2.empty());
}

// Test multimap copy semantics
TEST(MultimapTest, CopySemantics) {
    multimap<int, std::string> m1 = {{1, "one"}, {1, "uno"}};
    
    multimap<int, std::string> m2 = m1;
    EXPECT_EQ(m2.size(), m1.size());
    EXPECT_EQ(m2.count(1), 2);
    
    multimap<int, std::string> m3;
    m3 = m1;
    EXPECT_EQ(m3.size(), m1.size());
    EXPECT_EQ(m3.count(1), 2);
}

// Test multimap initializer list constructor
TEST(MultimapTest, InitializerListConstructor) {
    multimap<int, std::string> m = {{1, "one"}, {1, "uno"}, {2, "two"}};
    
    EXPECT_EQ(m.size(), 3);
    EXPECT_EQ(m.count(1), 2);
    EXPECT_EQ(m.count(2), 1);
}

// Test multimap clear operation
TEST(MultimapTest, ClearOperation) {
    multimap<int, std::string> m = {{1, "one"}, {1, "uno"}, {2, "two"}};
    
    EXPECT_FALSE(m.empty());
    EXPECT_EQ(m.size(), 3);
    
    m.clear();
    
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0);
}

// Test multimap max_size
TEST(MultimapTest, MaxSize) {
    multimap<int, std::string> m;
    EXPECT_GT(m.max_size(), 0);
}

// Test multimap key_comp and value_comp
TEST(MultimapTest, Comparators) {
    multimap<int, std::string> m;
    
    auto key_comp = m.key_comp();
    EXPECT_TRUE(key_comp(1, 2));
    EXPECT_FALSE(key_comp(2, 1));
    
    auto value_comp = m.value_comp();
    EXPECT_TRUE(value_comp({1, "a"}, {2, "b"}));
    EXPECT_FALSE(value_comp({2, "b"}, {1, "a"}));
}

// Test multimap complex scenario with many duplicates
TEST(MultimapTest, ComplexDuplicates) {
    multimap<std::string, int> m;
    
    // Insert multiple values for the same key
    m.insert({"apple", 1});
    m.insert({"banana", 2});
    m.insert({"apple", 3});
    m.insert({"cherry", 4});
    m.insert({"apple", 5});
    m.insert({"banana", 6});
    m.insert({"apple", 7});
    
    EXPECT_EQ(m.size(), 7);
    EXPECT_EQ(m.count("apple"), 4);
    EXPECT_EQ(m.count("banana"), 2);
    EXPECT_EQ(m.count("cherry"), 1);
    
    // Test equal_range for apple
    auto range = m.equal_range("apple");
    std::vector<int> apple_values;
    for (auto it = range.first; it != range.second; ++it) {
        apple_values.push_back(it->second);
    }
    
    EXPECT_EQ(apple_values.size(), 4);
    EXPECT_EQ(apple_values[0], 1);
    EXPECT_EQ(apple_values[1], 3);
    EXPECT_EQ(apple_values[2], 5);
    EXPECT_EQ(apple_values[3], 7);
}