#include <iostream>
#include "stl/container/multiset.hpp"

int main() {
    stl::multiset<int> ms;
    
    ms.insert(10);
    std::cout << "After first insert, size: " << ms.size() << std::endl;
    
    ms.insert(10);
    std::cout << "After second insert, size: " << ms.size() << std::endl;
    
    std::cout << "Count of 10: " << ms.count(10) << std::endl;
    
    std::cout << "Elements: ";
    for (const auto& item : ms) {
        std::cout << item << " ";
    }
    std::cout << std::endl;
    
    auto lb = ms.lower_bound(10);
    auto ub = ms.upper_bound(10);
    std::cout << "Lower bound: " << (lb != ms.end() ? std::to_string(*lb) : "end") << std::endl;
    std::cout << "Upper bound: " << (ub != ms.end() ? std::to_string(*ub) : "end") << std::endl;
    
    auto range = ms.equal_range(10);
    std::cout << "Equal range elements: ";
    int count = 0;
    auto it = range.first;
    auto end_it = range.second;
    std::cout << "Starting iteration..." << std::endl;
    std::cout << "First node ptr: " << it.get_node() << std::endl;
    std::cout << "End node ptr: " << end_it.get_node() << std::endl;
    std::cout << "First node right child: " << (it.get_node()->right ? "exists" : "nullptr") << std::endl;
    if (it.get_node()->right) {
        std::cout << "Right child value: " << it.get_node()->right->data << std::endl;
    }
    while (it != end_it) {
        std::cout << *it << " ";
        count++;
        auto prev = it;
        ++it;
        std::cout << "[advanced to ";
        if (it != end_it) {
            std::cout << *it << " (ptr: " << it.get_node() << ")";
        } else {
            std::cout << "end (ptr: " << it.get_node() << ")";
        }
        std::cout << "] ";
        if (it == prev) {
            std::cout << "(iterator not advancing!)";
            break;
        }
    }
    std::cout << "(count: " << count << ")" << std::endl;
    
    return 0;
}