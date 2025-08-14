#include <iostream>
#include <vector>
#include "include/stl/iterator.hpp"

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    stl::reverse_iterator<std::vector<int>::iterator> rbegin(vec.end());
    stl::reverse_iterator<std::vector<int>::iterator> rend(vec.begin());
    
    std::cout << "rbegin.base() position: " << rbegin.base() - vec.begin() << std::endl;
    std::cout << "rend.base() position: " << rend.base() - vec.begin() << std::endl;
    std::cout << "rbegin > rend: " << (rbegin > rend) << std::endl;
    std::cout << "rend < rbegin: " << (rend < rbegin) << std::endl;
    std::cout << "rbegin.base() > rend.base(): " << (rbegin.base() > rend.base()) << std::endl;
    
    return 0;
}