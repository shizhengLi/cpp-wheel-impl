# STL Queue 实现文档

## 概述

本项目实现了一个完整的 STL `queue` 容器适配器，遵循先进先出（FIFO）的原则。Queue 是容器适配器，它提供了对底层容器的受限接口，专门用于队列操作。

## 设计特点

### 数据结构
- **适配器模式**：queue 不是独立的容器，而是对现有容器的适配
- **默认底层容器**：使用 `deque<T>` 作为默认底层容器
- **可定制底层容器**：支持任何满足 SequenceContainer 要求的容器

### 时间复杂度
- **插入操作**：O(1) - `push()` 和 `emplace()` 操作
- **删除操作**：O(1) - `pop()` 操作
- **访问操作**：O(1) - `front()` 和 `back()` 操作
- **容量操作**：O(1) - `size()` 和 `empty()` 操作

### 空间复杂度
- **与底层容器相同**：空间复杂度取决于所使用的底层容器
- **无额外开销**：适配器本身不增加额外的空间开销

## 核心功能实现

### 1. 构造函数
- **默认构造**：创建空队列
- **拷贝构造**：从现有队列拷贝
- **移动构造**：从现有队列移动
- **容器构造**：从底层容器构造
- **分配器构造**：支持自定义分配器

### 2. 元素访问
- `front()`：访问队首元素，提供常量和非常量版本
- `back()`：访问队尾元素，提供常量和非常量版本
- 异常安全：对空队列的访问会抛出异常（通过底层容器）

### 3. 容量操作
- `empty()`：检查队列是否为空
- `size()`：获取队列中元素数量

### 4. 修改器
- **插入操作**：
  - `push(const T&)`：拷贝方式入队
  - `push(T&&)`：移动方式入队
  - `emplace(Args...)`：原地构造元素
- **删除操作**：
  - `pop()`：移除队首元素
- **交换操作**：
  - `swap()`：交换两个队列的内容

### 5. 底层容器访问
- `get_container()`：获取底层容器的引用
- 支持常量和非常量版本

### 6. 比较操作
- 支持标准比较运算符（==, !=）
- 比较基于底层容器的内容

## 底层容器支持

### 支持的容器类型
1. **deque**（默认）：双端队列，提供平衡的性能
2. **list**：双向链表，提供稳定的迭代器

### 容器要求
底层容器必须满足以下要求：
- `front()`：访问第一个元素
- `back()`：访问最后一个元素
- `push_back()`：在末尾添加元素
- `pop_front()`：删除第一个元素
- `empty()`：检查是否为空
- `size()`：获取元素数量

## 使用示例

### 基本使用
```cpp
#include "stl/adapter/queue.hpp"
#include <iostream>

using namespace stl;

int main() {
    queue<int> q;
    
    // 入队操作
    q.push(1);
    q.push(2);
    q.push(3);
    
    // 访问队首和队尾
    std::cout << "Front: " << q.front() << std::endl;  // 输出: 1
    std::cout << "Back: " << q.back() << std::endl;    // 输出: 3
    
    // 出队操作
    q.pop();
    std::cout << "Front after pop: " << q.front() << std::endl;  // 输出: 2
    
    // 检查状态
    std::cout << "Size: " << q.size() << std::endl;  // 输出: 2
    std::cout << "Empty: " << q.empty() << std::endl;  // 输出: 0
    
    return 0;
}
```

### 使用不同底层容器
```cpp
#include "stl/adapter/queue.hpp"
#include "stl/list.hpp"

using namespace stl;

int main() {
    // 使用 list 作为底层容器
    queue<int, list<int>> list_queue;
    list_queue.push(1);
    list_queue.push(2);
    list_queue.push(3);
    
    // 访问和操作
    while (!list_queue.empty()) {
        std::cout << "Front: " << list_queue.front() << std::endl;
        std::cout << "Back: " << list_queue.back() << std::endl;
        list_queue.pop();
    }
    
    return 0;
}
```

### 使用 emplace 操作
```cpp
#include "stl/adapter/queue.hpp"
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
    queue<Person> q;
    
    // 使用 emplace 原地构造对象
    q.emplace("Alice", 25);
    q.emplace("Bob", 30);
    q.emplace("Charlie", 35);
    
    // 访问和打印
    while (!q.empty()) {
        q.front().print();
        q.pop();
    }
    
    return 0;
}
```

## 测试覆盖

实现包含全面的测试用例，覆盖以下方面：

1. **基本操作**：构造、入队、出队、访问
2. **容量操作**：empty、size
3. **拷贝语义**：拷贝构造、拷贝赋值
4. **移动语义**：移动构造、移动赋值
5. **不同底层容器**：list 作为底层容器
6. **比较操作**：相等和不等比较
7. **交换操作**：swap 功能
8. **异常安全**：边界条件处理
9. **FIFO 行为**：先进先出特性验证
10. **字符串类型**：复杂数据类型支持
11. **front/back 一致性**：队首队尾访问一致性

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

2. **list**：
   - 优点：插入删除稳定，不失效其他元素
   - 缺点：内存开销大，缓存不友好

## 与标准 STL Queue 的差异

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

这个 queue 实现适用于：

1. **需要 FIFO 行为的场景**：如任务调度、消息队列
2. **算法实现**：如广度优先搜索、缓冲区管理
3. **事件处理**：如事件队列、请求处理
4. **教学目的**：理解适配器模式和队列的实现原理

对于需要高性能队列操作的场景，建议根据具体需求选择合适的底层容器。

## 扩展方向

如果需要进一步提升这个实现，可以考虑：

1. **更多异常安全保证**：实现强异常安全保证
2. **调试支持**：添加调试信息和边界检查
3. **性能优化**：针对特定场景进行优化
4. **更多 STL 兼容特性**：实现更多标准库兼容的功能
5. **分配器优化**：提供更灵活的分配器支持

## 总结

这个 queue 实现提供了完整的 STL queue 功能，具有良好的教学价值和实用性。通过理解这个实现，可以更好地掌握：

- 适配器设计模式
- STL 容器适配器的实现原理
- 模板编程和泛型设计
- 不同底层容器的性能特征
- 异常安全和资源管理

对于学习 C++ 和 STL 的开发者来说，这是一个有价值的参考实现。