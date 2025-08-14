# STL Stack 实现文档

## 概述

本项目实现了一个完整的 STL `stack` 容器适配器，遵循后进先出（LIFO）的原则。Stack 是容器适配器，它提供了对底层容器的受限接口，专门用于栈操作。

## 设计特点

### 数据结构
- **适配器模式**：stack 不是独立的容器，而是对现有容器的适配
- **默认底层容器**：使用 `deque<T>` 作为默认底层容器
- **可定制底层容器**：支持任何满足 SequenceContainer 要求的容器

### 时间复杂度
- **插入操作**：O(1) - `push()` 和 `emplace()` 操作
- **删除操作**：O(1) - `pop()` 操作
- **访问操作**：O(1) - `top()` 操作
- **容量操作**：O(1) - `size()` 和 `empty()` 操作

### 空间复杂度
- **与底层容器相同**：空间复杂度取决于所使用的底层容器
- **无额外开销**：适配器本身不增加额外的空间开销

## 核心功能实现

### 1. 构造函数
- **默认构造**：创建空栈
- **拷贝构造**：从现有栈拷贝
- **移动构造**：从现有栈移动
- **容器构造**：从底层容器构造
- **分配器构造**：支持自定义分配器

### 2. 元素访问
- `top()`：访问栈顶元素，提供常量和非常量版本
- 异常安全：对空栈的访问会抛出异常（通过底层容器）

### 3. 容量操作
- `empty()`：检查栈是否为空
- `size()`：获取栈中元素数量
- `max_size()`：获取栈的最大可能容量

### 4. 修改器
- **插入操作**：
  - `push(const T&)`：拷贝方式压栈
  - `push(T&&)`：移动方式压栈
  - `emplace(Args...)`：原地构造元素
- **删除操作**：
  - `pop()`：弹出栈顶元素
- **交换操作**：
  - `swap()`：交换两个栈的内容

### 5. 底层容器访问
- `get_container()`：获取底层容器的引用
- 支持常量和非常量版本

### 6. 比较操作
- 支持所有标准比较运算符（==, !=, <, <=, >, >=）
- 比较基于底层容器的内容

## 底层容器支持

### 支持的容器类型
1. **deque**（默认）：双端队列，提供平衡的性能
2. **vector**：动态数组，提供高效的内存局部性
3. **list**：双向链表，提供稳定的迭代器

### 容器要求
底层容器必须满足以下要求：
- `back()`：访问最后一个元素
- `push_back()`：在末尾添加元素
- `pop_back()`：删除最后一个元素
- `empty()`：检查是否为空
- `size()`：获取元素数量

## 使用示例

### 基本使用
```cpp
#include "stl/adapter/stack.hpp"
#include <iostream>

using namespace stl;

int main() {
    stack<int> s;
    
    // 压栈操作
    s.push(1);
    s.push(2);
    s.push(3);
    
    // 访问栈顶
    std::cout << "Top: " << s.top() << std::endl;  // 输出: 3
    
    // 弹栈操作
    s.pop();
    std::cout << "Top after pop: " << s.top() << std::endl;  // 输出: 2
    
    // 检查状态
    std::cout << "Size: " << s.size() << std::endl;  // 输出: 2
    std::cout << "Empty: " << s.empty() << std::endl;  // 输出: 0
    
    return 0;
}
```

### 使用不同底层容器
```cpp
#include "stl/adapter/stack.hpp"
#include "stl/vector.hpp"
#include "stl/list.hpp"

using namespace stl;

int main() {
    // 使用 vector 作为底层容器
    stack<int, vector<int>> vector_stack;
    vector_stack.push(1);
    vector_stack.push(2);
    vector_stack.push(3);
    
    // 使用 list 作为底层容器
    stack<int, list<int>> list_stack;
    list_stack.push(1);
    list_stack.push(2);
    list_stack.push(3);
    
    // 两个栈的行为相同，但底层实现不同
    while (!vector_stack.empty()) {
        std::cout << vector_stack.top() << " ";
        vector_stack.pop();
    }
    std::cout << std::endl;
    
    while (!list_stack.empty()) {
        std::cout << list_stack.top() << " ";
        list_stack.pop();
    }
    std::cout << std::endl;
    
    return 0;
}
```

### 使用 emplace 操作
```cpp
#include "stl/adapter/stack.hpp"
#include <string>

using namespace stl;

class Person {
public:
    Person(const std::string& name, int age) : name_(name), age_(age) {}
    
    void print() const {
        std::cout << "Name: " << name_ << ", Age: " << age_ << std::endl;
    }

private:
    std::string name_;
    int age_;
};

int main() {
    stack<Person> s;
    
    // 使用 emplace 原地构造对象
    s.emplace("Alice", 25);
    s.emplace("Bob", 30);
    s.emplace("Charlie", 35);
    
    // 访问和打印
    while (!s.empty()) {
        s.top().print();
        s.pop();
    }
    
    return 0;
}
```

## 测试覆盖

实现包含全面的测试用例，覆盖以下方面：

1. **基本操作**：构造、压栈、弹栈、访问
2. **容量操作**：empty、size、max_size
3. **拷贝语义**：拷贝构造、拷贝赋值
4. **移动语义**：移动构造、移动赋值
5. **不同底层容器**：vector、list、deque
6. **比较操作**：所有比较运算符
7. **交换操作**：swap 功能
8. **异常安全**：边界条件处理
9. **LIFO 行为**：后进先出特性验证
10. **字符串类型**：复杂数据类型支持

## 性能特征

### 优势
- **操作效率**：所有操作都是 O(1) 时间复杂度
- **灵活性**：可以适配不同的底层容器
- **内存效率**：适配器本身没有额外开销
- **异常安全**：提供基本的异常安全保证

### 不同底层容器的性能对比
1. **deque**：
   - 优点：平衡的性能，适合大多数场景
   - 缺点：内存不连续，可能影响缓存效率

2. **vector**：
   - 优点：内存连续，缓存友好
   - 缺点：扩容时可能需要重新分配

3. **list**：
   - 优点：插入删除稳定，不失效其他元素
   - 缺点：内存开销大，缓存不友好

## 与标准 STL Stack 的差异

### 相似之处
- 完全兼容的接口
- 相同的语义和行为
- 支持相同的底层容器
- 相同的性能特征

### 主要差异
1. **异常处理**：
   - 标准：可能有更复杂的异常安全保证
   - 本实现：依赖底层容器的异常处理

2. **分配器支持**：
   - 标准：可能有更完善的分配器支持
   - 本实现：提供基本的分配器支持

3. **优化程度**：
   - 标准：经过高度优化的工业级实现
   - 本实现：专注于功能正确性和教学目的

## 适用场景

这个 stack 实现适用于：

1. **需要 LIFO 行为的场景**：如函数调用栈、表达式求值
2. **算法实现**：如深度优先搜索、回溯算法
3. **临时数据存储**：如撤销操作、历史记录
4. **教学目的**：理解适配器模式和栈的实现原理

对于需要高性能栈操作的场景，建议根据具体需求选择合适的底层容器。

## 扩展方向

如果需要进一步提升这个实现，可以考虑：

1. **更多异常安全保证**：实现强异常安全保证
2. **调试支持**：添加调试信息和边界检查
3. **性能优化**：针对特定场景进行优化
4. **更多 STL 兼容特性**：实现更多标准库兼容的功能
5. **分配器优化**：提供更灵活的分配器支持

## 总结

这个 stack 实现提供了完整的 STL stack 功能，具有良好的教学价值和实用性。通过理解这个实现，可以更好地掌握：

- 适配器设计模式
- STL 容器适配器的实现原理
- 模板编程和泛型设计
- 不同底层容器的性能特征
- 异常安全和资源管理

对于学习 C++ 和 STL 的开发者来说，这是一个有价值的参考实现。