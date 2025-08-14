#include <gtest/gtest.h>
#include "stl/list.hpp"
#include <string>
#include <memory>

using namespace stl;

TEST(ListTest, BasicOperations) {
    list<int> lst;
    
    // Test push_back
    lst.push_back(1);
    EXPECT_EQ(lst.size(), 1);
    EXPECT_EQ(lst.front(), 1);
    EXPECT_EQ(lst.back(), 1);
    
    // Test push_front
    lst.push_front(0);
    EXPECT_EQ(lst.size(), 2);
    EXPECT_EQ(lst.front(), 0);
    EXPECT_EQ(lst.back(), 1);
    
    // Test iteration
    int sum = 0;
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 1);
    
    // Test pop
    lst.pop_front();
    EXPECT_EQ(lst.size(), 1);
    EXPECT_EQ(lst.front(), 1);
    
    lst.pop_back();
    EXPECT_TRUE(lst.empty());
}

TEST(ListTest, InsertAndErase) {
    list<int> lst = {1, 2, 3};
    
    // Insert at beginning
    auto it = lst.insert(lst.begin(), 0);
    EXPECT_EQ(lst.size(), 4);
    EXPECT_EQ(lst.front(), 0);
    
    // Insert in middle
    it = lst.begin();
    ++it;
    ++it;
    lst.insert(it, 99);
    EXPECT_EQ(lst.size(), 5);
    
    // Erase from middle
    it = lst.begin();
    ++it;
    ++it;
    it = lst.erase(it);
    EXPECT_EQ(lst.size(), 4);
    
    // Erase range
    lst.erase(lst.begin(), lst.end());
    EXPECT_TRUE(lst.empty());
}

TEST(ListTest, EmplaceOperations) {
    list<std::string> lst;
    
    // emplace_back
    lst.emplace_back("hello");
    EXPECT_EQ(lst.back(), "hello");
    
    // emplace_front
    lst.emplace_front("world");
    EXPECT_EQ(lst.front(), "world");
    
    // emplace in middle
    auto it = lst.begin();
    ++it;
    lst.emplace(it, "!");
    EXPECT_EQ(lst.size(), 3);
}

TEST(ListTest, Resize) {
    list<int> lst = {1, 2, 3};
    
    // Grow
    lst.resize(5, 0);
    EXPECT_EQ(lst.size(), 5);
    EXPECT_EQ(lst.back(), 0);
    
    // Shrink
    lst.resize(2);
    EXPECT_EQ(lst.size(), 2);
    EXPECT_EQ(lst.back(), 2);
}

TEST(ListTest, Splice) {
    list<int> lst1 = {1, 2, 3};
    list<int> lst2 = {4, 5, 6};
    
    // Splice entire list
    lst1.splice(lst1.end(), lst2);
    EXPECT_EQ(lst1.size(), 6);
    EXPECT_TRUE(lst2.empty());
    
    // Verify order
    int expected[] = {1, 2, 3, 4, 5, 6};
    int i = 0;
    for (const auto& val : lst1) {
        EXPECT_EQ(val, expected[i++]);
    }
}

TEST(ListTest, RemoveAndRemoveIf) {
    list<int> lst = {1, 2, 3, 2, 4, 2, 5};
    
    // Remove specific value
    lst.remove(2);
    EXPECT_EQ(lst.size(), 4);
    
    // Remove if even
    lst.remove_if([](int x) { return x % 2 == 0; });
    EXPECT_EQ(lst.size(), 3);  // 1, 3, 5 remain
}

TEST(ListTest, Reverse) {
    list<int> lst = {1, 2, 3, 4, 5};
    lst.reverse();
    
    int expected[] = {5, 4, 3, 2, 1};
    int i = 0;
    for (const auto& val : lst) {
        EXPECT_EQ(val, expected[i++]);
    }
}

TEST(ListTest, Unique) {
    list<int> lst = {1, 1, 2, 2, 2, 3, 4, 4, 4, 4};
    lst.unique();
    
    int expected[] = {1, 2, 3, 4};
    int i = 0;
    for (const auto& val : lst) {
        EXPECT_EQ(val, expected[i++]);
    }
}

TEST(ListTest, Sort) {
    list<int> lst = {5, 2, 8, 1, 9, 3};
    lst.sort();
    
    int expected[] = {1, 2, 3, 5, 8, 9};
    int i = 0;
    for (const auto& val : lst) {
        EXPECT_EQ(val, expected[i++]);
    }
}

TEST(ListTest, MoveSemantics) {
    list<std::unique_ptr<int>> lst;
    
    lst.push_back(std::make_unique<int>(42));
    EXPECT_EQ(*lst.front(), 42);
    
    auto ptr = std::make_unique<int>(100);
    lst.push_front(std::move(ptr));
    EXPECT_EQ(*lst.front(), 100);
    EXPECT_EQ(ptr.get(), nullptr);
}

TEST(ListTest, CopyOperations) {
    list<int> original = {1, 2, 3};
    
    // Copy constructor
    list<int> copy(original);
    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(copy.front(), 1);
    
    // Copy assignment
    list<int> assigned;
    assigned = original;
    EXPECT_EQ(assigned.size(), 3);
    EXPECT_EQ(assigned.front(), 1);
}

TEST(ListTest, InitializerList) {
    list<int> lst = {1, 2, 3, 4, 5};
    EXPECT_EQ(lst.size(), 5);
    EXPECT_EQ(lst.front(), 1);
    EXPECT_EQ(lst.back(), 5);
}

TEST(ListTest, ReverseIterators) {
    list<int> lst = {1, 2, 3, 4, 5};
    
    int expected[] = {5, 4, 3, 2, 1};
    int i = 0;
    for (auto it = lst.rbegin(); it != lst.rend(); ++it) {
        EXPECT_EQ(*it, expected[i++]);
    }
}

TEST(ListTest, ComparisonOperators) {
    list<int> lst1 = {1, 2, 3};
    list<int> lst2 = {1, 2, 3};
    list<int> lst3 = {1, 2, 4};
    
    EXPECT_EQ(lst1, lst2);
    EXPECT_NE(lst1, lst3);
    EXPECT_LT(lst1, lst3);
    EXPECT_GT(lst3, lst1);
}

TEST(ListTest, EdgeCases) {
    list<int> lst;
    
    // Test empty list operations
    EXPECT_TRUE(lst.empty());
    EXPECT_EQ(lst.size(), 0);
    
    // Test single element
    lst.push_back(42);
    EXPECT_EQ(lst.size(), 1);
    EXPECT_EQ(lst.front(), 42);
    EXPECT_EQ(lst.back(), 42);
    
    // Test clear
    lst.clear();
    EXPECT_TRUE(lst.empty());
}

TEST(ListTest, IteratorOperations) {
    list<int> lst = {1, 2, 3, 4, 5};
    
    // Test iterator arithmetic
    auto it = lst.begin();
    ++it;
    EXPECT_EQ(*it, 2);
    --it;
    EXPECT_EQ(*it, 1);
    
    // Test const iterator
    const list<int>& const_lst = lst;
    auto cit = const_lst.begin();
    EXPECT_EQ(*cit, 1);
    ++cit;
    EXPECT_EQ(*cit, 2);
}

TEST(ListTest, AllocatorAware) {
    list<int, stl::allocator<int>> lst;
    lst.push_back(1);
    lst.push_back(2);
    
    EXPECT_EQ(lst.size(), 2);
    EXPECT_EQ(lst.get_allocator(), stl::allocator<int>());
}