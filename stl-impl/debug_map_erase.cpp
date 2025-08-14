#include <iostream>
#include "stl/container/map.hpp"

int main() {
    stl::map<int, std::string> m = {{1, "one"}, {2, "two"}, {3, "three"}};
    
    std::cout << "Initial map contents:" << std::endl;
    for (const auto& kv : m) {
        std::cout << kv.first << " => " << kv.second << std::endl;
    }
    std::cout << "Size: " << m.size() << std::endl;
    
    std::cout << "\nErasing key 2..." << std::endl;
    auto result = m.erase(2);
    std::cout << "Erase result: " << result << std::endl;
    std::cout << "Size after erase: " << m.size() << std::endl;
    
    std::cout << "\nMap contents after erase:" << std::endl;
    for (const auto& kv : m) {
        std::cout << kv.first << " => " << kv.second << std::endl;
    }
    
    std::cout << "\nTrying to find key 3..." << std::endl;
    auto it = m.find(3);
    if (it != m.end()) {
        std::cout << "Found key 3: " << it->second << std::endl;
    } else {
        std::cout << "Key 3 not found!" << std::endl;
    }
    
    std::cout << "\nErasing key 3..." << std::endl;
    result = m.erase(3);
    std::cout << "Erase result: " << result << std::endl;
    std::cout << "Size after erase: " << m.size() << std::endl;
    
    return 0;
}