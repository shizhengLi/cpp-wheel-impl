# unique_ptr 实现详解

## 概述

`unique_ptr` 是C++11引入的智能指针，提供独占所有权的内存管理。它是C++标准库中最常用的智能指针之一，具有高效的性能和明确的所有权语义。

## 核心特性

### 1. 独占所有权
- 一个对象只能被一个`unique_ptr`拥有
- 不支持拷贝操作，防止所有权共享
- 支持移动操作，允许所有权转移

### 2. 自动内存管理
- 基于RAII（Resource Acquisition Is Initialization）原则
- 析构时自动释放管理的资源
- 防止内存泄漏

### 3. 高性能
- 零开销抽象，性能接近原始指针
- 没有引用计数的开销
- 编译器优化友好

## 实现架构

### 类模板设计

```cpp
template<typename T, typename Deleter = DefaultDeleter<T>>
class unique_ptr {
private:
    T* ptr_;          // 管理的原始指针
    Deleter deleter_; // 删除器对象

public:
    // 构造、析构、移动操作
    // 解引用操作符
    // 所有权管理方法
};
```

### 数组特化

```cpp
template<typename T, typename Deleter>
class unique_ptr<T[], Deleter> {
    // 数组特定的实现
    // 支持operator[]访问
};
```

## 核心实现细节

### 1. 构造函数

#### 默认构造函数
```cpp
constexpr unique_ptr() noexcept : ptr_(nullptr) {}
```

#### 指针构造函数
```cpp
explicit unique_ptr(T* ptr) noexcept : ptr_(ptr) {}
```

#### 移动构造函数
```cpp
unique_ptr(unique_ptr&& other) noexcept 
    : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
    other.ptr_ = nullptr;  // 原指针置空，避免重复释放
}
```

### 2. 删除拷贝语义

```cpp
unique_ptr(const unique_ptr&) = delete;
unique_ptr& operator=(const unique_ptr&) = delete;
```

这是`unique_ptr`的关键特性，确保独占所有权。

### 3. 移动赋值运算符

```cpp
unique_ptr& operator=(unique_ptr&& other) noexcept {
    if (this != &other) {  // 防止自赋值
        if (ptr_ != nullptr) {
            deleter_(ptr_);  // 释放当前资源
        }
        ptr_ = other.ptr_;
        deleter_ = std::move(other.deleter_);
        other.ptr_ = nullptr;  // 原指针置空
    }
    return *this;
}
```

### 4. 解引用操作符

```cpp
T& operator*() const {
    if (ptr_ == nullptr) {
        throw std::runtime_error("Dereferencing null unique_ptr");
    }
    return *ptr_;
}

T* operator->() const {
    if (ptr_ == nullptr) {
        throw std::runtime_error("Accessing null unique_ptr");
    }
    return ptr_;
}
```

### 5. 所有权管理方法

#### reset() 方法
```cpp
void reset(T* ptr = nullptr) noexcept {
    if (ptr_ != nullptr) {
        deleter_(ptr_);  // 释放当前资源
    }
    ptr_ = ptr;  // 管理新资源
}
```

#### release() 方法
```cpp
T* release() noexcept {
    T* temp = ptr_;
    ptr_ = nullptr;  // 放弃所有权
    return temp;     // 返回原始指针
}
```

### 6. 删除器设计

#### 默认删除器
```cpp
template<typename T>
struct DefaultDeleter {
    void operator()(T* ptr) const {
        delete ptr;
    }
};

// 数组特化
template<typename T>
struct DefaultDeleter<T[]> {
    void operator()(T* ptr) const {
        delete[] ptr;
    }
};
```

#### 自定义删除器支持
```cpp
template<typename T>
struct CustomDeleter {
    void operator()(T* ptr) const {
        // 自定义删除逻辑
        std::cout << "Custom deletion" << std::endl;
        delete ptr;
    }
};
```

### 7. 数组特化

```cpp
template<typename T, typename Deleter>
class unique_ptr<T[], Deleter> {
    // 数组下标访问
    T& operator[](std::size_t index) const {
        if (ptr_ == nullptr) {
            throw std::runtime_error("Accessing null unique_ptr array");
        }
        return ptr_[index];
    }
};
```

### 8. 工厂函数

#### make_unique
```cpp
template<typename T, typename... Args>
std::enable_if_t<!std::is_array_v<T>, unique_ptr<T>>
make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// 数组版本
template<typename T>
std::enable_if_t<std::is_array_v<T>, unique_ptr<T>>
make_unique(std::size_t size) {
    using U = std::remove_extent_t<T>;
    return unique_ptr<T>(new U[size]());
}
```

## 关键技术点

### 1. 移动语义
- 使用`std::move`转移资源所有权
- 原对象置空，避免重复释放
- 零拷贝的高效资源转移

### 2. 异常安全
- 构造函数标记为`noexcept`
- 解引用空指针时抛出异常
- 确保资源不会泄漏

### 3. 类型安全
- 使用`static_assert`防止误用
- 数组特化提供正确的删除行为
- SFINAE技术支持工厂函数

### 4. 完整性支持
- 实现所有标准操作符重载
- 支持与nullptr的比较
- 提供swap函数和比较运算符

## 使用示例

### 基本使用
```cpp
#include "unique_ptr.hpp"

class MyClass {
public:
    MyClass(int value) : value_(value) {}
    void print() const { std::cout << value_ << std::endl; }
private:
    int value_;
};

int main() {
    // 创建unique_ptr
    my::unique_ptr<MyClass> ptr(new MyClass(42));
    ptr->print();  // 输出: 42
    
    // 移动所有权
    my::unique_ptr<MyClass> ptr2 = std::move(ptr);
    ptr2->print();  // 输出: 42
    
    // 使用make_unique
    auto ptr3 = my::make_unique<MyClass>(100);
    ptr3->print();  // 输出: 100
    
    return 0;
}
```

### 数组支持
```cpp
int main() {
    // 数组unique_ptr
    my::unique_ptr<int[]> arr = my::make_unique<int[]>(5);
    
    for (int i = 0; i < 5; ++i) {
        arr[i] = i * 10;
    }
    
    for (int i = 0; i < 5; ++i) {
        std::cout << arr[i] << " ";  // 输出: 0 10 20 30 40
    }
    
    return 0;
}
```

### 自定义删除器
```cpp
struct FileDeleter {
    void operator()(FILE* file) const {
        if (file) {
            fclose(file);
            std::cout << "File closed" << std::endl;
        }
    }
};

int main() {
    my::unique_ptr<FILE, FileDeleter> file(fopen("test.txt", "r"));
    // 文件会在unique_ptr析构时自动关闭
    
    return 0;
}
```

## 测试策略

### 1. 功能测试
- 基本构造和析构
- 移动语义验证
- 所有权转移测试
- 异常处理测试

### 2. 边界测试
- 空指针操作
- 自赋值保护
- 内存泄漏检测
- 类型特征验证

### 3. 性能测试
- 与原始指针性能对比
- 内存使用效率
- 编译优化效果

## 性能分析

### 1. 内存开销
- 仅包含一个指针和一个删除器对象
- 默认删除器为空基类优化，零大小
- 总开销：8字节（64位系统）

### 2. 运行时开销
- 构造/析构：仅一次函数调用
- 解引用：与原始指针相同
- 移动操作：指针拷贝，零开销

### 3. 编译优化
- 内联优化消除函数调用开销
- 常量折叠和死代码消除
- 返回值优化（RVO）

## 常见问题与解决方案

### 1. 循环引用
`unique_ptr`不会产生循环引用问题，因为不支持拷贝。

### 2. 所有权混乱
始终使用`std::move`明确转移所有权，避免隐式拷贝。

### 3. 异常安全
确保构造函数不抛出异常，或使用`try-catch`块保护。

### 4. 线程安全
`unique_ptr`本身不是线程安全的，需要在应用层同步。

## 最佳实践

### 1. 优先使用make_unique
```cpp
// 推荐
auto ptr = my::make_unique<MyClass>(42);

// 不推荐
my::unique_ptr<MyClass> ptr(new MyClass(42));
```

### 2. 避免裸指针传递
```cpp
// 推荐
void process(my::unique_ptr<MyClass> ptr);

// 不推荐
void process(MyClass* raw_ptr);
```

### 3. 正确处理所有权转移
```cpp
my::unique_ptr<MyClass> source = my::make_unique<MyClass>(42);
my::unique_ptr<MyClass> target = std::move(source);  // 明确转移
```

### 4. 合理使用自定义删除器
```cpp
// 资源管理
my::unique_ptr<FILE, FileDeleter> file(fopen("test.txt", "r"));

// 内存池管理
my::unique_ptr<int, PoolDeleter> ptr(pool.allocate(), PoolDeleter(pool));
```

## 总结

`unique_ptr`的实现展示了C++模板编程、RAII原则、移动语义等核心概念的正确应用。通过精心设计的接口和实现，我们创建了一个既安全又高效的智能指针，为现代C++编程提供了重要的内存管理工具。

这个实现不仅涵盖了`unique_ptr`的所有核心功能，还提供了完整的测试用例和详细的使用文档，可以作为学习C++高级特性的优秀示例。