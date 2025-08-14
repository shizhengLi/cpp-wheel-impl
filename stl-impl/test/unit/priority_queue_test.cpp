#include <gtest/gtest.h>
#include "stl/adapter/priority_queue.hpp"
#include "stl/vector.hpp"
#include "stl/deque.hpp"
#include <string>
#include <memory>

using namespace stl;

TEST(PriorityQueueTest, BasicOperations) {
    priority_queue<int> pq;
    
    // Test empty queue
    EXPECT_TRUE(pq.empty());
    EXPECT_EQ(pq.size(), 0);
    
    // Test push
    pq.push(3);
    EXPECT_FALSE(pq.empty());
    EXPECT_EQ(pq.size(), 1);
    EXPECT_EQ(pq.top(), 3);
    
    pq.push(1);
    EXPECT_EQ(pq.size(), 2);
    EXPECT_EQ(pq.top(), 3);  // Max element at top
    
    pq.push(4);
    EXPECT_EQ(pq.size(), 3);
    EXPECT_EQ(pq.top(), 4);  // New max element at top
    
    pq.push(2);
    EXPECT_EQ(pq.size(), 4);
    EXPECT_EQ(pq.top(), 4);
    
    // Test pop
    pq.pop();
    EXPECT_EQ(pq.size(), 3);
    EXPECT_EQ(pq.top(), 3);
    
    pq.pop();
    EXPECT_EQ(pq.size(), 2);
    EXPECT_EQ(pq.top(), 2);
    
    pq.pop();
    EXPECT_EQ(pq.size(), 1);
    EXPECT_EQ(pq.top(), 1);
    
    pq.pop();
    EXPECT_TRUE(pq.empty());
}

TEST(PriorityQueueTest, Emplace) {
    priority_queue<std::string> pq;
    
    pq.emplace("hello");
    EXPECT_EQ(pq.size(), 1);
    EXPECT_EQ(pq.top(), "hello");
    
    pq.emplace("world");
    EXPECT_EQ(pq.size(), 2);
    EXPECT_EQ(pq.top(), "world");  // "world" > "hello"
    
    pq.emplace("apple");
    EXPECT_EQ(pq.size(), 3);
    EXPECT_EQ(pq.top(), "world");  // "world" is still largest
    
    pq.emplace("zebra");
    EXPECT_EQ(pq.size(), 4);
    EXPECT_EQ(pq.top(), "zebra");  // "zebra" is largest
}

TEST(PriorityQueueTest, CopyOperations) {
    priority_queue<int> original;
    original.push(1);
    original.push(3);
    original.push(2);
    
    // Copy constructor
    priority_queue<int> copy(original);
    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(copy.top(), 3);
    
    // Copy assignment
    priority_queue<int> assigned;
    assigned = original;
    EXPECT_EQ(assigned.size(), 3);
    EXPECT_EQ(assigned.top(), 3);
    
    // Verify original is unchanged
    EXPECT_EQ(original.size(), 3);
    EXPECT_EQ(original.top(), 3);
}

TEST(PriorityQueueTest, MoveOperations) {
    priority_queue<std::string> pq;
    pq.push("hello");
    pq.push("world");
    
    // Move constructor
    priority_queue<std::string> moved(std::move(pq));
    EXPECT_EQ(moved.size(), 2);
    EXPECT_EQ(moved.top(), "world");
    EXPECT_TRUE(pq.empty());
    
    // Move assignment
    priority_queue<std::string> assigned;
    assigned = std::move(moved);
    EXPECT_EQ(assigned.size(), 2);
    EXPECT_EQ(assigned.top(), "world");
    EXPECT_TRUE(moved.empty());
}

TEST(PriorityQueueTest, CustomComparator) {
    // Min-heap using greater
    priority_queue<int, vector<int>, greater<int>> min_pq;
    
    min_pq.push(3);
    min_pq.push(1);
    min_pq.push(4);
    min_pq.push(2);
    
    EXPECT_EQ(min_pq.top(), 1);  // Min element at top
    
    min_pq.pop();
    EXPECT_EQ(min_pq.top(), 2);
    
    min_pq.pop();
    EXPECT_EQ(min_pq.top(), 3);
    
    min_pq.pop();
    EXPECT_EQ(min_pq.top(), 4);
}

TEST(PriorityQueueTest, RangeConstructor) {
    std::vector<int> vec = {3, 1, 4, 1, 5, 9, 2, 6};
    
    // Range constructor
    priority_queue<int> pq(vec.begin(), vec.end());
    EXPECT_EQ(pq.size(), 8);
    EXPECT_EQ(pq.top(), 9);  // Max element
    
    // Test that it's properly heapified
    pq.pop();
    EXPECT_EQ(pq.top(), 6);
    pq.pop();
    EXPECT_EQ(pq.top(), 5);
}

TEST(PriorityQueueTest, DifferentContainers) {
    // Test with different vector types
    priority_queue<int, vector<int>> vector_pq;
    
    vector_pq.push(3);
    vector_pq.push(1);
    vector_pq.push(4);
    vector_pq.push(2);
    
    EXPECT_EQ(vector_pq.size(), 4);
    EXPECT_EQ(vector_pq.top(), 4);
    
    vector_pq.pop();
    EXPECT_EQ(vector_pq.top(), 3);
    vector_pq.pop();
    EXPECT_EQ(vector_pq.top(), 2);
}

TEST(PriorityQueueTest, ComparisonOperators) {
    priority_queue<int> pq1;
    priority_queue<int> pq2;
    priority_queue<int> pq3;
    
    pq1.push(1);
    pq1.push(2);
    pq1.push(3);
    
    pq2.push(1);
    pq2.push(2);
    pq2.push(3);
    
    pq3.push(1);
    pq3.push(2);
    pq3.push(4);
    
    EXPECT_EQ(pq1, pq2);
    EXPECT_NE(pq1, pq3);
}

TEST(PriorityQueueTest, Swap) {
    priority_queue<int> pq1;
    priority_queue<int> pq2;
    
    pq1.push(1);
    pq1.push(3);
    pq1.push(2);
    
    pq2.push(5);
    pq2.push(4);
    
    pq1.swap(pq2);
    
    EXPECT_EQ(pq1.size(), 2);
    EXPECT_EQ(pq1.top(), 5);
    
    EXPECT_EQ(pq2.size(), 3);
    EXPECT_EQ(pq2.top(), 3);
}

TEST(PriorityQueueTest, GetContainer) {
    priority_queue<int> pq;
    pq.push(1);
    pq.push(3);
    pq.push(2);
    
    // Test get container
    auto& container = pq.get_container();
    EXPECT_EQ(container.size(), 3);
    
    // Test const get container
    const priority_queue<int>& const_pq = pq;
    const auto& const_container = const_pq.get_container();
    EXPECT_EQ(const_container.size(), 3);
}

TEST(PriorityQueueTest, GetComparator) {
    priority_queue<int> pq;
    
    // Test get comparator
    auto& comp = pq.get_comparator();
    EXPECT_TRUE(comp(1, 2));  // 1 < 2
    
    // Test const get comparator
    const priority_queue<int>& const_pq = pq;
    const auto& const_comp = const_pq.get_comparator();
    EXPECT_TRUE(const_comp(1, 2));
}

TEST(PriorityQueueTest, EdgeCases) {
    priority_queue<int> pq;
    
    // Test single element
    pq.push(42);
    EXPECT_EQ(pq.size(), 1);
    EXPECT_EQ(pq.top(), 42);
    
    // Test pop single element
    pq.pop();
    EXPECT_TRUE(pq.empty());
    
    // Test push after empty
    pq.push(100);
    EXPECT_EQ(pq.size(), 1);
    EXPECT_EQ(pq.top(), 100);
    
    // Test duplicate elements
    pq.push(100);
    pq.push(100);
    pq.push(100);
    EXPECT_EQ(pq.size(), 4);
    EXPECT_EQ(pq.top(), 100);
    
    pq.pop();
    EXPECT_EQ(pq.top(), 100);
    pq.pop();
    EXPECT_EQ(pq.top(), 100);
    pq.pop();
    EXPECT_EQ(pq.top(), 100);
    pq.pop();
    EXPECT_TRUE(pq.empty());
}

TEST(PriorityQueueTest, LargeDataset) {
    priority_queue<int> pq;
    
    // Insert many elements
    for (int i = 0; i < 1000; ++i) {
        pq.push(i);
    }
    
    EXPECT_EQ(pq.size(), 1000);
    EXPECT_EQ(pq.top(), 999);
    
    // Remove all elements and verify order
    for (int i = 999; i >= 0; --i) {
        EXPECT_EQ(pq.top(), i);
        pq.pop();
    }
    
    EXPECT_TRUE(pq.empty());
}

TEST(PriorityQueueTest, StringPriorityQueue) {
    priority_queue<std::string> pq;
    
    pq.push("apple");
    pq.push("banana");
    pq.push("cherry");
    pq.push("date");
    
    EXPECT_EQ(pq.size(), 4);
    EXPECT_EQ(pq.top(), "date");  // "date" is lexicographically largest
    
    pq.pop();
    EXPECT_EQ(pq.top(), "cherry");
    pq.pop();
    EXPECT_EQ(pq.top(), "banana");
    pq.pop();
    EXPECT_EQ(pq.top(), "apple");
    pq.pop();
    EXPECT_TRUE(pq.empty());
}

TEST(PriorityQueueTest, CustomObjectPriorityQueue) {
    struct Person {
        std::string name;
        int age;
        
        Person(const std::string& n, int a) : name(n), age(a) {}
        
        bool operator<(const Person& other) const {
            return age < other.age;  // Age-based comparison
        }
    };
    
    priority_queue<Person> pq;
    
    pq.emplace("Alice", 25);
    pq.emplace("Bob", 30);
    pq.emplace("Charlie", 20);
    
    EXPECT_EQ(pq.size(), 3);
    EXPECT_EQ(pq.top().name, "Bob");  // Bob is oldest
    EXPECT_EQ(pq.top().age, 30);
    
    pq.pop();
    EXPECT_EQ(pq.top().name, "Alice");
    EXPECT_EQ(pq.top().age, 25);
    
    pq.pop();
    EXPECT_EQ(pq.top().name, "Charlie");
    EXPECT_EQ(pq.top().age, 20);
}