#include <gtest/gtest.h>
#include "stl/deque.hpp"

using namespace stl;

TEST(DequeMinimalTest, BasicOperations) {
    deque<int> d;
    
    // Test push_back
    d.push_back(1);
    EXPECT_EQ(d.size(), 1);
    EXPECT_EQ(d.front(), 1);
    EXPECT_EQ(d.back(), 1);
    
    // Test push_front
    d.push_front(0);
    EXPECT_EQ(d.size(), 2);
    EXPECT_EQ(d.front(), 0);
    EXPECT_EQ(d.back(), 1);
    
    // Test iteration
    int sum = 0;
    for (auto it = d.begin(); it != d.end(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 1);
    
    // Test pop
    d.pop_front();
    EXPECT_EQ(d.size(), 1);
    EXPECT_EQ(d.front(), 1);
    
    d.pop_back();
    EXPECT_TRUE(d.empty());
}

TEST(DequeMinimalTest, StringTest) {
    deque<std::string> d;
    
    d.push_back("hello");
    d.push_front("world");
    
    EXPECT_EQ(d.size(), 2);
    EXPECT_EQ(d.front(), "world");
    EXPECT_EQ(d.back(), "hello");
}

TEST(DequeMinimalTest, MoveTest) {
    deque<std::unique_ptr<int>> d;
    
    d.push_back(std::make_unique<int>(42));
    EXPECT_EQ(*d[0], 42);
    
    auto ptr = std::make_unique<int>(100);
    d.push_front(std::move(ptr));
    EXPECT_EQ(*d[0], 100);
    EXPECT_EQ(ptr.get(), nullptr);
}