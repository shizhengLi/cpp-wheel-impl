#include <iostream>
#include <vector>
#include <iterator>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    std::reverse_iterator<std::vector<int>::iterator> rbegin(vec.end());
    std::reverse_iterator<std::vector<int>::iterator> rend(vec.begin());
    
    std::cout << "rbegin.base() position: " << rbegin.base() - vec.begin() << std::endl;
    std::cout << "rend.base() position: " << rend.base() - vec.begin() << std::endl;
    std::cout << "rbegin > rend: " << (rbegin > rend) << std::endl;
    std::cout << "rend < rbegin: " << (rend < rbegin) << std::endl;
    std::cout << "*rbegin: " << *rbegin << std::endl;
    
    return 0;
}