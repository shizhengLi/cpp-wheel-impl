#include <gtest/gtest.h>
#include "stl/adapter/stack.hpp"
#include "stl/vector.hpp"
#include "stl/list.hpp"
#include <string>

using namespace stl;

TEST(StackTest, BasicOperations) {
    stack<int> s;
    
    // Test empty stack
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
    
    // Test push
    s.push(1);
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s.top(), 1);
    
    s.push(2);
    EXPECT_EQ(s.size(), 2);
    EXPECT_EQ(s.top(), 2);
    
    s.push(3);
    EXPECT_EQ(s.size(), 3);
    EXPECT_EQ(s.top(), 3);
    
    // Test pop
    s.pop();
    EXPECT_EQ(s.size(), 2);
    EXPECT_EQ(s.top(), 2);
    
    s.pop();
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s.top(), 1);
    
    s.pop();
    EXPECT_TRUE(s.empty());
}

TEST(StackTest, Emplace) {
    stack<std::string> s;
    
    s.emplace("hello");
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s.top(), "hello");
    
    s.emplace("world");
    EXPECT_EQ(s.size(), 2);
    EXPECT_EQ(s.top(), "world");
}

TEST(StackTest, CopyOperations) {
    stack<int> original;
    original.push(1);
    original.push(2);
    original.push(3);
    
    // Copy constructor
    stack<int> copy(original);
    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(copy.top(), 3);
    
    // Copy assignment
    stack<int> assigned;
    assigned = original;
    EXPECT_EQ(assigned.size(), 3);
    EXPECT_EQ(assigned.top(), 3);
}

TEST(StackTest, MoveOperations) {
    stack<std::unique_ptr<int>> s;
    s.push(std::make_unique<int>(42));
    
    // Move constructor
    stack<std::unique_ptr<int>> moved(std::move(s));
    EXPECT_EQ(moved.size(), 1);
    EXPECT_EQ(*moved.top(), 42);
    EXPECT_TRUE(s.empty());
    
    // Move assignment
    stack<std::unique_ptr<int>> assigned;
    assigned = std::move(moved);
    EXPECT_EQ(assigned.size(), 1);
    EXPECT_EQ(*assigned.top(), 42);
    EXPECT_TRUE(moved.empty());
}

TEST(StackTest, DifferentContainers) {
    // Test with vector
    stack<int, vector<int>> vector_stack;
    vector_stack.push(1);
    vector_stack.push(2);
    vector_stack.push(3);
    
    EXPECT_EQ(vector_stack.size(), 3);
    EXPECT_EQ(vector_stack.top(), 3);
    
    vector_stack.pop();
    EXPECT_EQ(vector_stack.size(), 2);
    EXPECT_EQ(vector_stack.top(), 2);
    
    // Test with list
    stack<int, list<int>> list_stack;
    list_stack.push(1);
    list_stack.push(2);
    list_stack.push(3);
    
    EXPECT_EQ(list_stack.size(), 3);
    EXPECT_EQ(list_stack.top(), 3);
    
    list_stack.pop();
    EXPECT_EQ(list_stack.size(), 2);
    EXPECT_EQ(list_stack.top(), 2);
}

TEST(StackTest, ComparisonOperators) {
    stack<int> s1;
    stack<int> s2;
    stack<int> s3;
    
    s1.push(1);
    s1.push(2);
    s1.push(3);
    
    s2.push(1);
    s2.push(2);
    s2.push(3);
    
    s3.push(1);
    s3.push(2);
    s3.push(4);
    
    EXPECT_EQ(s1, s2);
    EXPECT_NE(s1, s3);
    EXPECT_LT(s1, s3);
    EXPECT_GT(s3, s1);
    EXPECT_LE(s1, s2);
    EXPECT_GE(s2, s1);
}

TEST(StackTest, Swap) {
    stack<int> s1;
    stack<int> s2;
    
    s1.push(1);
    s1.push(2);
    s1.push(3);
    
    s2.push(4);
    s2.push(5);
    
    s1.swap(s2);
    
    EXPECT_EQ(s1.size(), 2);
    EXPECT_EQ(s1.top(), 5);
    
    EXPECT_EQ(s2.size(), 3);
    EXPECT_EQ(s2.top(), 3);
}

TEST(StackTest, GetContainer) {
    stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);
    
    // Test get container
    auto& container = s.get_container();
    EXPECT_EQ(container.size(), 3);
    EXPECT_EQ(container.back(), 3);
    
    // Test const get container
    const stack<int>& const_s = s;
    const auto& const_container = const_s.get_container();
    EXPECT_EQ(const_container.size(), 3);
    EXPECT_EQ(const_container.back(), 3);
}

TEST(StackTest, EdgeCases) {
    stack<int> s;
    
    // Test single element
    s.push(42);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s.top(), 42);
    
    // Test pop single element
    s.pop();
    EXPECT_TRUE(s.empty());
    
    // Test push after empty
    s.push(100);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s.top(), 100);
}

TEST(StackTest, LIFOBehavior) {
    stack<int> s;
    
    // Test Last In First Out behavior
    s.push(1);
    s.push(2);
    s.push(3);
    s.push(4);
    s.push(5);
    
    EXPECT_EQ(s.top(), 5);
    s.pop();
    EXPECT_EQ(s.top(), 4);
    s.pop();
    EXPECT_EQ(s.top(), 3);
    s.pop();
    EXPECT_EQ(s.top(), 2);
    s.pop();
    EXPECT_EQ(s.top(), 1);
    s.pop();
    EXPECT_TRUE(s.empty());
}

TEST(StackTest, StringStack) {
    stack<std::string> s;
    
    s.push("hello");
    s.push("world");
    s.push("!");
    
    EXPECT_EQ(s.size(), 3);
    EXPECT_EQ(s.top(), "!");
    
    s.pop();
    EXPECT_EQ(s.top(), "world");
    
    s.pop();
    EXPECT_EQ(s.top(), "hello");
    
    s.pop();
    EXPECT_TRUE(s.empty());
}