#include <iostream>
#include <vector>
#include "stl/iterator.hpp"

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    auto it = vec.begin();
    
    std::cout << "Initial: " << *it << std::endl;
    stl::advance(it, 3);
    std::cout << "After advance(3): " << *it << std::endl;
    stl::advance(it, -2);
    std::cout << "After advance(-2): " << *it << std::endl;
    
    // Test next and prev
    auto it2 = vec.begin() + 2;
    std::cout << "it2 points to: " << *it2 << std::endl;
    auto next_it = stl::next(it2);
    std::cout << "next(it2) points to: " << *next_it << std::endl;
    auto prev_it = stl::prev(it2);
    std::cout << "prev(it2) points to: " << *prev_it << std::endl;
    
    // Test if it's random access iterator
    using category = stl::iterator_traits<std::vector<int>::iterator>::iterator_category;
    std::cout << "Is random access (stl): " << std::is_base_of_v<stl::random_access_iterator_tag, category> << std::endl;
    std::cout << "Is random access (std): " << std::is_base_of_v<std::random_access_iterator_tag, category> << std::endl;
    
    return 0;
}