#include <iostream>
#include <vector>
#include <iterator>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    std::reverse_iterator<std::vector<int>::iterator> r1(vec.end());
    std::reverse_iterator<std::vector<int>::iterator> r3(vec.begin());
    
    std::cout << "r1.base() position: " << r1.base() - vec.begin() << std::endl;
    std::cout << "r3.base() position: " << r3.base() - vec.begin() << std::endl;
    std::cout << "r3 < r1: " << (r3 < r1) << std::endl;
    std::cout << "r1 > r3: " << (r1 > r3) << std::endl;
    
    return 0;
}