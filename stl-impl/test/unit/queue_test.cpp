#include <gtest/gtest.h>
#include "stl/adapter/queue.hpp"
#include "stl/vector.hpp"
#include "stl/list.hpp"
#include <string>

using namespace stl;

TEST(QueueTest, BasicOperations) {
    queue<int> q;
    
    // Test empty queue
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(q.size(), 0);
    
    // Test push
    q.push(1);
    EXPECT_FALSE(q.empty());
    EXPECT_EQ(q.size(), 1);
    EXPECT_EQ(q.front(), 1);
    EXPECT_EQ(q.back(), 1);
    
    q.push(2);
    EXPECT_EQ(q.size(), 2);
    EXPECT_EQ(q.front(), 1);
    EXPECT_EQ(q.back(), 2);
    
    q.push(3);
    EXPECT_EQ(q.size(), 3);
    EXPECT_EQ(q.front(), 1);
    EXPECT_EQ(q.back(), 3);
    
    // Test pop
    q.pop();
    EXPECT_EQ(q.size(), 2);
    EXPECT_EQ(q.front(), 2);
    EXPECT_EQ(q.back(), 3);
    
    q.pop();
    EXPECT_EQ(q.size(), 1);
    EXPECT_EQ(q.front(), 3);
    EXPECT_EQ(q.back(), 3);
    
    q.pop();
    EXPECT_TRUE(q.empty());
}

TEST(QueueTest, Emplace) {
    queue<std::string> q;
    
    q.emplace("hello");
    EXPECT_EQ(q.size(), 1);
    EXPECT_EQ(q.front(), "hello");
    EXPECT_EQ(q.back(), "hello");
    
    q.emplace("world");
    EXPECT_EQ(q.size(), 2);
    EXPECT_EQ(q.front(), "hello");
    EXPECT_EQ(q.back(), "world");
}

TEST(QueueTest, CopyOperations) {
    queue<int> original;
    original.push(1);
    original.push(2);
    original.push(3);
    
    // Copy constructor
    queue<int> copy(original);
    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(copy.front(), 1);
    EXPECT_EQ(copy.back(), 3);
    
    // Copy assignment
    queue<int> assigned;
    assigned = original;
    EXPECT_EQ(assigned.size(), 3);
    EXPECT_EQ(assigned.front(), 1);
    EXPECT_EQ(assigned.back(), 3);
}

TEST(QueueTest, MoveOperations) {
    queue<std::unique_ptr<int>> q;
    q.push(std::make_unique<int>(42));
    
    // Move constructor
    queue<std::unique_ptr<int>> moved(std::move(q));
    EXPECT_EQ(moved.size(), 1);
    EXPECT_EQ(*moved.front(), 42);
    EXPECT_EQ(*moved.back(), 42);
    EXPECT_TRUE(q.empty());
    
    // Move assignment
    queue<std::unique_ptr<int>> assigned;
    assigned = std::move(moved);
    EXPECT_EQ(assigned.size(), 1);
    EXPECT_EQ(*assigned.front(), 42);
    EXPECT_EQ(*assigned.back(), 42);
    EXPECT_TRUE(moved.empty());
}

TEST(QueueTest, DifferentContainers) {
    // Test with list (supports front and back operations)
    queue<int, list<int>> list_queue;
    list_queue.push(1);
    list_queue.push(2);
    list_queue.push(3);
    
    EXPECT_EQ(list_queue.size(), 3);
    EXPECT_EQ(list_queue.front(), 1);
    EXPECT_EQ(list_queue.back(), 3);
    
    list_queue.pop();
    EXPECT_EQ(list_queue.size(), 2);
    EXPECT_EQ(list_queue.front(), 2);
    EXPECT_EQ(list_queue.back(), 3);
}

TEST(QueueTest, ComparisonOperators) {
    queue<int> q1;
    queue<int> q2;
    queue<int> q3;
    
    q1.push(1);
    q1.push(2);
    q1.push(3);
    
    q2.push(1);
    q2.push(2);
    q2.push(3);
    
    q3.push(1);
    q3.push(2);
    q3.push(4);
    
    EXPECT_EQ(q1, q2);
    EXPECT_NE(q1, q3);
}

TEST(QueueTest, Swap) {
    queue<int> q1;
    queue<int> q2;
    
    q1.push(1);
    q1.push(2);
    q1.push(3);
    
    q2.push(4);
    q2.push(5);
    
    q1.swap(q2);
    
    EXPECT_EQ(q1.size(), 2);
    EXPECT_EQ(q1.front(), 4);
    EXPECT_EQ(q1.back(), 5);
    
    EXPECT_EQ(q2.size(), 3);
    EXPECT_EQ(q2.front(), 1);
    EXPECT_EQ(q2.back(), 3);
}

TEST(QueueTest, GetContainer) {
    queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    
    // Test get container
    auto& container = q.get_container();
    EXPECT_EQ(container.size(), 3);
    EXPECT_EQ(container.front(), 1);
    EXPECT_EQ(container.back(), 3);
    
    // Test const get container
    const queue<int>& const_q = q;
    const auto& const_container = const_q.get_container();
    EXPECT_EQ(const_container.size(), 3);
    EXPECT_EQ(const_container.front(), 1);
    EXPECT_EQ(const_container.back(), 3);
}

TEST(QueueTest, EdgeCases) {
    queue<int> q;
    
    // Test single element
    q.push(42);
    EXPECT_EQ(q.size(), 1);
    EXPECT_EQ(q.front(), 42);
    EXPECT_EQ(q.back(), 42);
    
    // Test pop single element
    q.pop();
    EXPECT_TRUE(q.empty());
    
    // Test push after empty
    q.push(100);
    EXPECT_EQ(q.size(), 1);
    EXPECT_EQ(q.front(), 100);
    EXPECT_EQ(q.back(), 100);
}

TEST(QueueTest, FIFOBehavior) {
    queue<int> q;
    
    // Test First In First Out behavior
    q.push(1);
    q.push(2);
    q.push(3);
    q.push(4);
    q.push(5);
    
    EXPECT_EQ(q.front(), 1);
    EXPECT_EQ(q.back(), 5);
    q.pop();
    EXPECT_EQ(q.front(), 2);
    EXPECT_EQ(q.back(), 5);
    q.pop();
    EXPECT_EQ(q.front(), 3);
    EXPECT_EQ(q.back(), 5);
    q.pop();
    EXPECT_EQ(q.front(), 4);
    EXPECT_EQ(q.back(), 5);
    q.pop();
    EXPECT_EQ(q.front(), 5);
    EXPECT_EQ(q.back(), 5);
    q.pop();
    EXPECT_TRUE(q.empty());
}

TEST(QueueTest, StringQueue) {
    queue<std::string> q;
    
    q.push("hello");
    q.push("world");
    q.push("!");
    
    EXPECT_EQ(q.size(), 3);
    EXPECT_EQ(q.front(), "hello");
    EXPECT_EQ(q.back(), "!");
    
    q.pop();
    EXPECT_EQ(q.front(), "world");
    EXPECT_EQ(q.back(), "!");
    
    q.pop();
    EXPECT_EQ(q.front(), "!");
    EXPECT_EQ(q.back(), "!");
    
    q.pop();
    EXPECT_TRUE(q.empty());
}

TEST(QueueTest, FrontBackConsistency) {
    queue<int> q;
    
    // Test that front and back are consistent for single element
    q.push(42);
    EXPECT_EQ(q.front(), 42);
    EXPECT_EQ(q.back(), 42);
    
    // Test with multiple elements
    q.push(100);
    EXPECT_EQ(q.front(), 42);
    EXPECT_EQ(q.back(), 100);
    
    q.push(200);
    EXPECT_EQ(q.front(), 42);
    EXPECT_EQ(q.back(), 200);
    
    // Test after pop
    q.pop();
    EXPECT_EQ(q.front(), 100);
    EXPECT_EQ(q.back(), 200);
    
    q.pop();
    EXPECT_EQ(q.front(), 200);
    EXPECT_EQ(q.back(), 200);
}