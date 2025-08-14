#include <iostream>
#include <vector>
#include "include/stl/iterator.hpp"

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    stl::reverse_iterator<std::vector<int>::iterator> rit(vec.end());
    
    std::cout << "rit points to: " << *rit << std::endl;
    std::cout << "rit.base() is at position: " << rit.base() - vec.begin() << std::endl;
    
    auto rit_minus = rit - 1;
    std::cout << "rit - 1 base() is at position: " << rit_minus.base() - vec.begin() << std::endl;
    
    // This is undefined behavior, but let's see what happens
    if (rit_minus.base() != vec.end()) {
        std::cout << "rit - 1 points to: " << *rit_minus << std::endl;
    } else {
        std::cout << "rit - 1 base() is at vec.end()" << std::endl;
    }
    
    return 0;
}