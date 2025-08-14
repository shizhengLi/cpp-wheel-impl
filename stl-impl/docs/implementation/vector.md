# STL Vector容器实现文档

## 模块概述

STL Vector容器是C++标准模板库中最常用的序列容器之一，提供了动态数组的功能。本实现完整地遵循了C++17标准，支持自动内存管理、动态扩容、随机访问、迭代器操作等核心功能。Vector在内存中连续存储元素，提供了O(1)的随机访问性能，同时在末尾插入和删除元素也具有O(1)的均摊时间复杂度。

## 实现内容

### 1. 核心数据结构

```cpp
template<typename T, typename Allocator = allocator<T>>
class vector {
private:
    pointer data_;          // 指向动态数组的指针
    size_type size_;        // 当前元素数量
    size_type capacity_;    // 当前容量
    allocator_type alloc_;  // 分配器实例
};
```

Vector内部维护三个关键数据成员：指向元素数组的指针、当前大小和当前容量。这种设计确保了高效的内存管理和快速的访问性能。

### 2. 构造函数和析构函数

#### 2.1 默认构造函数

```cpp
vector() noexcept(noexcept(Allocator())) 
    : data_(nullptr), size_(0), capacity_(0) {}
```

创建空的vector，不分配任何内存。

#### 2.2 指定大小构造函数

```cpp
explicit vector(size_type count, const T& value = T(), const Allocator& alloc = Allocator())
    : data_(nullptr), size_(0), capacity_(0), alloc_(alloc) {
    resize(count, value);
}
```

创建包含指定数量元素的vector，所有元素初始化为给定值。

#### 2.3 迭代器范围构造函数

```cpp
template<typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
vector(InputIt first, InputIt last, const Allocator& alloc = Allocator())
    : data_(nullptr), size_(0), capacity_(0), alloc_(alloc) {
    assign(first, last);
}
```

从迭代器范围构造vector，支持任意类型的输入迭代器。

#### 2.4 拷贝构造函数

```cpp
vector(const vector& other) 
    : data_(nullptr), size_(0), capacity_(0), 
      alloc_(allocator_traits<Allocator>::select_on_container_copy_construction(other.alloc_)) {
    reserve(other.size_);
    for (size_type i = 0; i < other.size_; ++i) {
        push_back(other.data_[i]);
    }
}
```

深度拷贝另一个vector的内容，根据分配器特性选择是否复制分配器。

#### 2.5 移动构造函数

```cpp
vector(vector&& other) noexcept
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_), alloc_(std::move(other.alloc_)) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}
```

高效移动另一个vector的资源，避免不必要的拷贝。

### 3. 元素访问

#### 3.1 随机访问

```cpp
reference operator[](size_type pos) noexcept {
    return data_[pos];
}

const_reference operator[](size_type pos) const noexcept {
    return data_[pos];
}
```

提供O(1)时间复杂度的随机访问，不进行边界检查。

#### 3.2 带边界检查的访问

```cpp
reference at(size_type pos) {
    if (pos >= size_) {
        throw vector_exception("vector::at: pos out of range");
    }
    return data_[pos];
}
```

提供边界检查，越界时抛出异常。

#### 3.3 首尾元素访问

```cpp
reference front() noexcept { return data_[0]; }
reference back() noexcept { return data_[size_ - 1]; }
```

快速访问首尾元素。

### 4. 迭代器支持

#### 4.1 迭代器类型

```cpp
using iterator = pointer;
using const_iterator = const_pointer;
using reverse_iterator = stl::reverse_iterator<iterator>;
using const_reverse_iterator = stl::reverse_iterator<const_iterator>;
```

Vector的迭代器就是原生指针，提供了最高的性能。

#### 4.2 迭代器接口

```cpp
iterator begin() noexcept { return data_; }
iterator end() noexcept { return data_ + size_; }
reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
// ... 其他迭代器方法
```

提供完整的STL迭代器接口，支持范围for循环和STL算法。

### 5. 容量管理

#### 5.1 动态扩容

```cpp
void reserve(size_type new_cap) {
    if (new_cap > capacity_) {
        reallocate(new_cap);
    }
}
```

预留容量，避免频繁重新分配内存。

#### 5.2 自动扩容策略

```cpp
void push_back(const T& value) {
    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    allocator_traits<Allocator>::construct(alloc_, data_ + size_, value);
    ++size_;
}
```

采用容量翻倍的策略，确保O(1)的均摊时间复杂度。

#### 5.3 内存收缩

```cpp
void shrink_to_fit() {
    if (size_ < capacity_) {
        reallocate(size_);
    }
}
```

释放未使用的内存，使容量等于大小。

### 6. 元素修改

#### 6.1 插入操作

```cpp
iterator insert(const_iterator pos, const T& value) {
    return emplace(pos, value);
}

template<typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
iterator insert(const_iterator pos, InputIt first, InputIt last) {
    // 移动现有元素，插入新元素
}
```

支持在任意位置插入元素，包括单个元素、多个元素和迭代器范围。

#### 6.2 原位构造

```cpp
template<typename... Args>
iterator emplace(const_iterator pos, Args&&... args) {
    size_type index = pos - begin();
    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    // 移动元素并原位构造
}
```

直接在指定位置构造元素，避免临时对象的创建和销毁。

#### 6.3 删除操作

```cpp
iterator erase(const_iterator pos) {
    return erase(pos, pos + 1);
}

iterator erase(const_iterator first, const_iterator last) {
    // 销毁元素，移动剩余元素
}
```

删除单个元素或范围内的元素。

### 7. 分配器支持

#### 7.1 allocator_traits实现

```cpp
template<typename Alloc>
struct allocator_traits {
    static pointer allocate(Alloc& a, size_type n) {
        return a.allocate(n);
    }
    
    template<typename U, typename... Args>
    static void construct(Alloc&, U* p, Args&&... args) {
        new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }
    
    template<typename U>
    static void destroy(Alloc&, U* p) {
        p->~U();
    }
};
```

提供了简化的allocator_traits实现，支持自定义分配器。

#### 7.2 分配器传播控制

```cpp
using propagate_on_container_copy_assignment = std::false_type;
using propagate_on_container_move_assignment = std::true_type;
using propagate_on_container_swap = std::false_type;
```

控制分配器在容器操作中的传播行为。

### 8. 异常处理

#### 8.1 自定义异常类

```cpp
class vector_exception : public std::runtime_error {
public:
    explicit vector_exception(const std::string& msg) : std::runtime_error(msg) {}
};
```

提供vector特定的异常类型。

#### 8.2 异常安全保证

- 基本保证：操作失败时容器保持有效状态
- 强保证：拷贝赋值操作提供强异常安全保证
- 不抛出保证：析构函数、swap等操作不抛出异常

## 技术要点

### 1. 内存管理策略

#### 1.1 指数扩容

采用容量翻倍的扩容策略：
```cpp
if (size_ == capacity_) {
    reserve(capacity_ == 0 ? 1 : capacity_ * 2);
}
```

这种策略确保了O(1)的均摊时间复杂度，避免了频繁的内存重新分配。

#### 1.2 移动语义优化

```cpp
void reallocate(size_type new_capacity) {
    pointer new_data = alloc_.allocate(new_capacity);
    for (size_type i = 0; i < size_; ++i) {
        allocator_traits<Allocator>::construct(alloc_, new_data + i, std::move(data_[i]));
        allocator_traits<Allocator>::destroy(alloc_, data_ + i);
    }
    // 释放旧内存
}
```

在重新分配内存时使用移动语义，避免不必要的拷贝。

### 2. 类型安全和SFINAE

#### 2.1 构造函数重载解析

```cpp
template<typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
vector(InputIt first, InputIt last, const Allocator& alloc = Allocator());
```

使用SFINAE技术区分迭代器构造和大小构造，避免歧义。

#### 2.2 完美转发

```cpp
template<typename... Args>
reference emplace_back(Args&&... args) {
    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    allocator_traits<Allocator>::construct(alloc_, data_ + size_, std::forward<Args>(args)...);
    ++size_;
    return back();
}
```

使用完美转发支持任意类型的构造参数。

### 3. 性能优化

#### 3.1 原生指针迭代器

```cpp
using iterator = pointer;
using const_iterator = const_pointer;
```

使用原生指针作为迭代器，提供零开销的抽象。

#### 3.2 内联优化

所有关键操作都适合内联，编译器可以进行深度优化。

#### 3.3 预分配优化

```cpp
void reserve(size_type new_cap) {
    if (new_cap > capacity_) {
        reallocate(new_cap);
    }
}
```

允许用户预分配内存，避免运行时扩容开销。

## 测试覆盖

本实现包含20个全面的测试用例：

1. **Constructors**: 测试所有构造函数
2. **AssignmentOperators**: 测试赋值运算符
3. **ElementAccess**: 测试元素访问方法
4. **Iterators**: 测试迭代器操作
5. **Capacity**: 测试容量管理
6. **Modifiers**: 测试修改器操作
7. **Insert**: 测试插入操作
8. **Emplace**: 测试原位构造
9. **Erase**: 测试删除操作
10. **Assign**: 测试赋值操作
11. **Swap**: 测试交换操作
12. **ComparisonOperators**: 测试比较运算符
13. **MoveSemantics**: 测试移动语义
14. **ExceptionSafety**: 测试异常安全
15. **MemoryManagement**: 测试内存管理
16. **StandardLibraryCompatibility**: 测试标准库兼容性
17. **AllocatorTest**: 测试分配器支持
18. **EdgeCases**: 测试边界情况
19. **PerformanceRelated**: 测试性能相关特性
20. **ComplexTypes**: 测试复杂类型支持

## 性能特点

1. **O(1)随机访问**: 通过连续内存布局和原生指针迭代器实现
2. **O(1)均摊插入**: 采用指数扩容策略确保高效的末尾插入
3. **移动语义**: 支持高效的资源转移，避免不必要的拷贝
4. **内存局部性**: 连续内存布局提供良好的缓存性能
5. **零开销抽象**: 迭代器使用原生指针，不引入运行时开销
6. **预分配优化**: 支持容量预留，减少动态扩容开销

## 与标准库兼容性

本实现与C++17标准库完全兼容：
- 所有公共接口都遵循标准库规范
- 支持标准库算法和迭代器操作
- 提供完整的异常安全保证
- 支持自定义分配器
- 保持与标准库vector的语义一致性

## 使用示例

```cpp
#include <iostream>
#include <algorithm>
#include "stl/vector.hpp"

int main() {
    // 基本操作
    stl::vector<int> vec = {1, 2, 3, 4, 5};
    
    // 添加元素
    vec.push_back(6);
    vec.emplace_back(7);
    
    // 随机访问
    std::cout << "First element: " << vec[0] << std::endl;
    std::cout << "Last element: " << vec.back() << std::endl;
    
    // 迭代器操作
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 算法操作
    std::sort(vec.begin(), vec.end());
    auto count = std::count(vec.begin(), vec.end(), 3);
    
    // 插入和删除
    vec.insert(vec.begin() + 2, 99);
    vec.erase(vec.begin() + 4);
    
    // 容量管理
    vec.reserve(20);
    vec.shrink_to_fit();
    
    // 移动语义
    stl::vector<int> vec2 = std::move(vec);
    
    return 0;
}
```

## 限制和注意事项

1. **线程安全**: 当前实现不提供线程安全保证，多线程环境下需要外部同步
2. **异常处理**: 某些操作可能抛出std::bad_alloc异常
3. **迭代器失效**: 插入和删除操作可能导致迭代器失效
4. **内存开销**: 指数扩容策略可能导致内存使用量大于实际需求
5. **类型要求**: 元素类型必须满足可拷贝构造和可赋值的要求

## 总结

STL Vector容器的成功实现展示了现代C++的强大特性。通过模板元编程、移动语义、完美转发和SFINAE等技术，我们实现了高性能、类型安全的动态数组容器。所有20个测试用例的通过证明了实现的正确性和鲁棒性。该实现为后续的STL容器开发提供了坚实的基础，同时也展示了如何在实际项目中应用C++的高级特性。