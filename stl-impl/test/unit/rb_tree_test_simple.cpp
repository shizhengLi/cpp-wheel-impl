#include <gtest/gtest.h>
#include "stl/tree/rb_tree.hpp"
#include "stl/functional.hpp"
#include <string>
#include <vector>

using namespace stl;

struct Identity {
    template <typename T>
    const T& operator()(const T& x) const {
        return x;
    }
};

TEST(RBTreeTest, BasicOperations) {
    rb_tree<int, int, Identity, less<int>, allocator<int>> tree;
    
    // Test empty tree
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0);
    EXPECT_EQ(tree.begin(), tree.end());
    
    // Test insert
    auto [it1, success1] = tree.insert(5);
    EXPECT_TRUE(success1);
    EXPECT_EQ(*it1, 5);
    EXPECT_EQ(tree.size(), 1);
    
    auto [it2, success2] = tree.insert(3);
    EXPECT_TRUE(success2);
    EXPECT_EQ(*it2, 3);
    EXPECT_EQ(tree.size(), 2);
    
    auto [it3, success3] = tree.insert(7);
    EXPECT_TRUE(success3);
    EXPECT_EQ(*it3, 7);
    EXPECT_EQ(tree.size(), 3);
    
    // Test duplicate insert
    auto [it4, success4] = tree.insert(3);
    EXPECT_FALSE(success4);
    EXPECT_EQ(*it4, 3);
    EXPECT_EQ(tree.size(), 3);
    
    // Test find
    auto found = tree.find(5);
    EXPECT_NE(found, tree.end());
    EXPECT_EQ(*found, 5);
    
    auto not_found = tree.find(10);
    EXPECT_EQ(not_found, tree.end());
    
    // Test basic iteration
    std::vector<int> elements;
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        elements.push_back(*it);
    }
    
    // Should be sorted: 3, 5, 7
    EXPECT_EQ(elements.size(), 3);
    EXPECT_EQ(elements[0], 3);
    EXPECT_EQ(elements[1], 5);
    EXPECT_EQ(elements[2], 7);
    
    // Test RB tree properties
    EXPECT_TRUE(tree.is_valid_rb_tree());
}

TEST(RBTreeTest, LargeDataset) {
    rb_tree<int, int, Identity, less<int>, allocator<int>> tree;
    
    // Insert many elements
    for (int i = 0; i < 100; ++i) {
        auto [it, success] = tree.insert(i);
        EXPECT_TRUE(success);
    }
    
    EXPECT_EQ(tree.size(), 100);
    
    // Verify all elements are present and sorted
    int expected = 0;
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        EXPECT_EQ(*it, expected);
        ++expected;
    }
    
    EXPECT_EQ(expected, 100);
    EXPECT_TRUE(tree.is_valid_rb_tree());
}

TEST(RBTreeTest, StringKeys) {
    rb_tree<std::string, std::string, Identity, less<std::string>, allocator<std::string>> tree;
    
    tree.insert("apple");
    tree.insert("banana");
    tree.insert("cherry");
    
    EXPECT_EQ(tree.size(), 3);
    EXPECT_TRUE(tree.is_valid_rb_tree());
    
    // Test iteration order
    auto it = tree.begin();
    EXPECT_EQ(*it, "apple");
    ++it;
    EXPECT_EQ(*it, "banana");
    ++it;
    EXPECT_EQ(*it, "cherry");
    ++it;
    EXPECT_EQ(it, tree.end());
}

TEST(RBTreeTest, Clear) {
    rb_tree<int, int, Identity, less<int>, allocator<int>> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    
    EXPECT_EQ(tree.size(), 3);
    EXPECT_FALSE(tree.empty());
    
    tree.clear();
    
    EXPECT_EQ(tree.size(), 0);
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.begin(), tree.end());
    EXPECT_TRUE(tree.is_valid_rb_tree());
}