# STL Priority Queue 实现文档

## 概述

本项目实现了一个完整的 STL `priority_queue` 容器适配器，遵循优先级队列的原则。Priority queue 是容器适配器，它提供了对底层容器的受限接口，专门用于优先级队列操作，确保最高优先级的元素始终位于队首。

## 设计特点

### 数据结构
- **适配器模式**：priority_queue 不是独立的容器，而是对现有容器的适配
- **默认底层容器**：使用 `vector<T>` 作为默认底层容器
- **堆算法**：基于最大堆实现，确保最高优先级元素在队首
- **可定制比较器**：支持自定义比较器来定义优先级

### 时间复杂度
- **插入操作**：O(log n) - `push()` 和 `emplace()` 操作
- **删除操作**：O(log n) - `pop()` 操作
- **访问操作**：O(1) - `top()` 操作
- **容量操作**：O(1) - `size()` 和 `empty()` 操作

### 空间复杂度
- **与底层容器相同**：空间复杂度取决于所使用的底层容器
- **无额外开销**：适配器本身不增加额外的空间开销

## 核心功能实现

### 1. 构造函数
- **默认构造**：创建空优先队列
- **拷贝构造**：从现有优先队列拷贝
- **移动构造**：从现有优先队列移动
- **范围构造**：从迭代器范围构造
- **比较器构造**：支持自定义比较器
- **容器构造**：从底层容器构造

### 2. 元素访问
- `top()`：访问队首元素（最高优先级），提供常量版本
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

### 6. 比较器访问
- `get_comparator()`：获取比较器的引用
- 支持常量和非常量版本

### 7. 比较操作
- 支持标准比较运算符（==, !=）
- 比较基于底层容器的内容

## 堆算法实现

### 核心算法
- **make_heap**：构建堆结构
- **push_heap**：插入元素后调整堆
- **pop_heap**：删除元素前调整堆
- **sort_heap**：堆排序
- **is_heap**：检查堆性质

### 算法复杂度
- **make_heap**：O(n) - 线性时间构建堆
- **push_heap**：O(log n) - 向上调整堆
- **pop_heap**：O(log n) - 向下调整堆
- **is_heap**：O(n) - 检查堆性质

### 类型安全
- 使用 static_assert 确保只使用随机访问迭代器
- 编译时检查容器类型的兼容性

## 底层容器支持

### 支持的容器类型
1. **vector**（默认）：动态数组，提供高效的随机访问
2. **deque**：双端队列，提供随机访问（需要完整的随机访问迭代器支持）

### 容器要求
底层容器必须满足以下要求：
- **随机访问迭代器**：支持高效的随机访问
- `front()`：访问第一个元素
- `push_back()`：在末尾添加元素
- `pop_back()`：删除最后一个元素
- `empty()`：检查是否为空
- `size()`：获取元素数量

## 比较器支持

### 内置比较器
- `less<T>`：默认比较器，实现最大堆
- `greater<T>`：实现最小堆
- `equal_to<T>`：相等比较
- 其他标准比较器

### 自定义比较器
```cpp
struct CustomCompare {
    bool operator()(const T& a, const T& b) const {
        return a.custom_field < b.custom_field;
    }
};

priority_queue<T, vector<T>, CustomCompare> pq;
```

## 使用示例

### 基本使用
```cpp
#include "stl/adapter/priority_queue.hpp"
#include <iostream>

using namespace stl;

int main() {
    priority_queue<int> pq;
    
    // 入队操作
    pq.push(3);
    pq.push(1);
    pq.push(4);
    pq.push(2);
    
    // 访问队首（最大元素）
    std::cout << "Top: " << pq.top() << std::endl;  // 输出: 4
    
    // 出队操作
    pq.pop();
    std::cout << "Top after pop: " << pq.top() << std::endl;  // 输出: 3
    
    // 检查状态
    std::cout << "Size: " << pq.size() << std::endl;  // 输出: 3
    std::cout << "Empty: " << pq.empty() << std::endl;  // 输出: 0
    
    return 0;
}
```

### 最小堆实现
```cpp
#include "stl/adapter/priority_queue.hpp"
#include "stl/functional.hpp"
#include <iostream>

using namespace stl;

int main() {
    // 使用 greater 作为比较器，实现最小堆
    priority_queue<int, vector<int>, greater<int>> min_pq;
    
    min_pq.push(3);
    min_pq.push(1);
    min_pq.push(4);
    min_pq.push(2);
    
    // 访问队首（最小元素）
    std::cout << "Min: " << min_pq.top() << std::endl;  // 输出: 1
    
    // 出队操作
    min_pq.pop();
    std::cout << "Next min: " << min_pq.top() << std::endl;  // 输出: 2
    
    return 0;
}
```

### 使用 emplace 操作
```cpp
#include "stl/adapter/priority_queue.hpp"
#include <string>

using namespace stl;

class Task {
public:
    Task(const std::string& name, int priority) : name_(name), priority_(priority) {}
    
    bool operator<(const Task& other) const {
        return priority_ < other.priority_;
    }
    
    void print() const {
        std::cout << "Task: " << name_ << ", Priority: " << priority_ << std::endl;
    }

private:
    std::string name_;
    int priority_;
};

int main() {
    priority_queue<Task> pq;
    
    // 使用 emplace 原地构造对象
    pq.emplace("Write report", 2);
    pq.emplace("Fix bug", 5);
    pq.emplace("Meeting", 3);
    
    // 按优先级访问和打印
    while (!pq.empty()) {
        pq.top().print();
        pq.pop();
    }
    
    return 0;
}
```

### 范围构造
```cpp
#include "stl/adapter/priority_queue.hpp"
#include <vector>
#include <iostream>

using namespace stl;

int main() {
    std::vector<int> vec = {3, 1, 4, 1, 5, 9, 2, 6};
    
    // 使用范围构造函数
    priority_queue<int> pq(vec.begin(), vec.end());
    
    // 输出排序结果
    while (!pq.empty()) {
        std::cout << pq.top() << " ";
        pq.pop();
    }
    std::cout << std::endl;  // 输出: 9 6 5 4 3 2 1 1
    
    return 0;
}
```

## 测试覆盖

实现包含全面的测试用例，覆盖以下方面：

1. **基本操作**：构造、入队、出队、访问
2. **容量操作**：empty、size
3. **拷贝语义**：拷贝构造、拷贝赋值
4. **移动语义**：移动构造、移动赋值
5. **自定义比较器**：greater 比较器测试
6. **范围构造**：迭代器范围构造
7. **不同底层容器**：vector 和 deque
8. **比较操作**：相等和不等比较
9. **交换操作**：swap 功能
10. **异常安全**：边界条件处理
11. **堆性质**：优先级正确性验证
12. **大数据集**：性能和正确性测试
13. **字符串类型**：复杂数据类型支持
14. **自定义对象**：用户定义类型支持

## 性能特征

### 优势
- **高效访问**：最高优先级元素访问 O(1)
- **对数复杂度**：插入和删除操作 O(log n)
- **灵活性**：可以适配不同的底层容器
- **可定制性**：支持自定义比较器
- **内存效率**：适配器本身没有额外开销

### 不同底层容器的性能对比
1. **vector**：
   - 优点：内存连续，缓存友好，随机访问高效
   - 缺点：插入时可能需要重新分配内存

2. **deque**：
   - 优点：两端操作高效，内存分配灵活
   - 缺点：内存不连续，可能影响缓存效率

## 与标准 STL Priority Queue 的差异

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

这个 priority_queue 实现适用于：

1. **需要优先级访问的场景**：如任务调度、事件处理
2. **算法实现**：如 Dijkstra 算法、A* 算法
3. **数据流处理**：如数据流中的最大值/最小值查找
4. **堆排序**：作为堆排序的基础数据结构
5. **教学目的**：理解适配器模式和堆的实现原理

对于需要高性能优先队列操作的场景，建议根据具体需求选择合适的底层容器和比较器。

## 扩展方向

如果需要进一步提升这个实现，可以考虑：

1. **更多异常安全保证**：实现强异常安全保证
2. **调试支持**：添加调试信息和边界检查
3. **性能优化**：针对特定场景进行优化
4. **更多 STL 兼容特性**：实现更多标准库兼容的功能
5. **分配器优化**：提供更灵活的分配器支持
6. **迭代器支持**：添加迭代器接口（非标准但有用）
7. **合并操作**：实现优先队列的合并操作

## 总结

这个 priority_queue 实现提供了完整的 STL priority_queue 功能，具有良好的教学价值和实用性。通过理解这个实现，可以更好地掌握：

- 适配器设计模式
- 堆数据结构的实现原理
- 模板编程和泛型设计
- 不同底层容器的性能特征
- 异常安全和资源管理
- 算法复杂度分析

对于学习 C++ 和 STL 的开发者来说，这是一个有价值的参考实现。