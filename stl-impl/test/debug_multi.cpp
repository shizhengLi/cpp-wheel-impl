#include <iostream>
#include "stl/container/unordered_multiset.hpp"

int main() {
    stl::unordered_multiset<int> set;
    
    // Test basic insertion
    set.insert(10);
    set.insert(20);
    set.insert(10);  // duplicate
    set.insert(10);  // duplicate
    set.insert(30);
    set.insert(20);  // duplicate
    
    std::cout << "Size: " << set.size() << std::endl;
    std::cout << "Count of 10: " << set.count(10) << std::endl;
    std::cout << "Count of 20: " << set.count(20) << std::endl;
    std::cout << "Count of 30: " << set.count(30) << std::endl;
    
    // Test iteration
    std::cout << "Elements: ";
    for (auto it = set.begin(); it != set.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // Test equal_range
    auto range = set.equal_range(10);
    std::cout << "Equal range for 10: ";
    for (auto it = range.first; it != range.second; ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    return 0;
}