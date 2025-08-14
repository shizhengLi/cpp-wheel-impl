#include <iostream>
#include <vector>
#include "include/stl/iterator.hpp"

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    stl::reverse_iterator<std::vector<int>::iterator> r1(vec.end());
    stl::reverse_iterator<std::vector<int>::iterator> r2(vec.begin());
    
    std::cout << "r1.base() position: " << r1.base() - vec.begin() << std::endl;
    std::cout << "r2.base() position: " << r2.base() - vec.begin() << std::endl;
    std::cout << "r1.base() < r2.base(): " << (r1.base() < r2.base()) << std::endl;
    std::cout << "r1 > r2: " << (r1 > r2) << std::endl;
    std::cout << "r2 < r1: " << (r2 < r1) << std::endl;
    
    // Test with the same type
    using RevIt = stl::reverse_iterator<std::vector<int>::iterator>;
    RevIt r3(vec.end());
    RevIt r4(vec.begin());
    std::cout << "r3 > r4: " << (r3 > r4) << std::endl;
    
    return 0;
}