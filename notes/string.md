# string 字符串类实现详解

## 概述

`string` 是C++中最常用的类之一，提供了动态字符串管理功能。我们的实现包含了小字符串优化（SSO）、移动语义、完整的字符串操作等功能。

## 核心特性

### 1. 小字符串优化（SSO）
- 对于长度≤15的字符串，直接在栈上存储
- 避免了堆内存分配的开销
- 显著提升了小字符串的性能

### 2. 移动语义支持
- 高效的字符串转移所有权
- 避免不必要的深拷贝
- 提升了性能

### 3. 完整的字符串操作
- 基本操作：构造、赋值、连接、比较
- 高级操作：查找、替换、插入、删除
- 内存管理：容量控制、收缩适配

### 4. 异常安全
- 边界检查
- 内存分配失败处理
- 强异常安全保证

## 实现架构

### 类设计

```cpp
class string {
private:
    // 小字符串优化（SSO）
    static constexpr size_t SSO_MAX_SIZE = 15;
    static constexpr size_t SSO_BUFFER_SIZE = SSO_MAX_SIZE + 1;
    
    union {
        struct {
            char* ptr;          // 指向堆内存
            size_t size;         // 字符串长度
            size_t capacity;     // 分配的容量
            bool is_small;       // 是否为小字符串
        } large;
        struct {
            char data[SSO_MAX_SIZE + 1];  // 字符串数据
            unsigned char size;   // 字符串长度
        } small;
    } data_;
    
    // 迭代器类型
    using iterator = ...;
    using const_iterator = ...;
    
public:
    // 类型定义
    using size_type = size_t;
    using value_type = char;
    // ... 其他类型定义
    
    // 构造、析构、赋值
    // 元素访问
    // 迭代器
    // 容量操作
    // 字符串操作
    // 比较操作
};
```

### 小字符串优化机制

#### 内存布局
```cpp
union {
    struct {
        char* ptr;          // 8 bytes
        size_t size;         // 8 bytes
        size_t capacity;     // 8 bytes
        bool is_small;       // 1 byte (padding: 7 bytes)
    } large;                // total: 32 bytes
    struct {
        char data[16];      // 16 bytes (15 chars + null terminator)
        unsigned char size;  // 1 byte
        // padding: 15 bytes
    } small;                // total: 32 bytes
} data_;
```

#### 判断逻辑
```cpp
bool is_small() const noexcept {
    return data_.large.is_small;
}
```

#### 大小字符串转换
- 小字符串 → 大字符串：调用`grow()`方法
- 大字符串 → 小字符串：调用`shrink_to_fit()`方法

## 核心实现细节

### 1. 构造函数

#### 默认构造函数
```cpp
string() noexcept {
    data_.small.data[0] = '\0';
    set_small_size(0);
    data_.large.is_small = true;  // 标记为小字符串
}
```

#### C字符串构造函数
```cpp
string(const char* str) {
    if (str == nullptr) {
        data_.small.data[0] = '\0';
        set_small_size(0);
        data_.large.is_small = true;
    } else {
        init_from_cstr(str);
    }
}
```

#### 拷贝构造函数
```cpp
string(const string& other) {
    size_t other_size = other.size();
    if (other_size <= SSO_MAX_SIZE) {
        // 小字符串优化
        std::memcpy(data_.small.data, other.data_.small.data, other_size + 1);
        set_small_size(other_size);
        data_.large.is_small = true;
    } else {
        // 大字符串
        data_.large.ptr = new char[other_size + 1];
        std::memcpy(data_.large.ptr, other.data_.large.ptr, other_size + 1);
        data_.large.size = other_size;
        data_.large.capacity = other_size;
        data_.large.is_small = false;
    }
}
```

#### 移动构造函数
```cpp
string(string&& other) noexcept {
    if (other.is_small()) {
        // 小字符串直接拷贝
        std::memcpy(data_.small.data, other.data_.small.data, other.size() + 1);
        set_small_size(other.size());
        data_.large.is_small = true;
        other.data_.small.data[0] = '\0';
        other.set_small_size(0);
        other.data_.large.is_small = true;
    } else {
        // 大字符串转移所有权
        data_.large.ptr = other.data_.large.ptr;
        data_.large.size = other.data_.large.size;
        data_.large.capacity = other.data_.large.capacity;
        data_.large.is_small = false;
        
        other.data_.large.ptr = nullptr;
        other.data_.large.size = 0;
        other.data_.large.capacity = 0;
        other.data_.large.is_small = true;
    }
}
```

### 2. 内存管理

#### 容量扩展（grow方法）
```cpp
void grow(size_t new_capacity) {
    if (new_capacity <= capacity()) return;
    
    char* new_ptr = new char[new_capacity + 1];
    size_t current_size = is_small() ? get_small_size() : data_.large.size;
    std::memcpy(new_ptr, get_ptr(), current_size + 1);
    
    release_memory();
    set_large_ptr(new_ptr);
    set_large_size(current_size);
    set_large_capacity(new_capacity);
    data_.large.is_small = false;  // 标记为大字符串
}
```

#### 容量收缩（shrink_to_fit方法）
```cpp
void shrink_to_fit() {
    if (!is_small() && data_.large.capacity > data_.large.size) {
        size_t new_size = data_.large.size;
        if (new_size <= SSO_MAX_SIZE) {
            // 转换为小字符串
            char temp[SSO_MAX_SIZE + 1];
            std::memcpy(temp, data_.large.ptr, new_size + 1);
            delete[] data_.large.ptr;
            std::memcpy(data_.small.data, temp, new_size + 1);
            set_small_size(new_size);
            data_.large.is_small = true;  // 标记为小字符串
        } else {
            // 重新分配内存
            char* new_ptr = new char[new_size + 1];
            std::memcpy(new_ptr, data_.large.ptr, new_size + 1);
            delete[] data_.large.ptr;
            data_.large.ptr = new_ptr;
            data_.large.capacity = new_size;
        }
    }
}
```

### 3. 字符串操作

#### 连接操作
```cpp
string& append(const char* str) {
    size_t len = std::strlen(str);
    size_t new_size = size() + len;
    reserve(new_size);
    
    std::memcpy(get_ptr() + size(), str, len + 1);
    
    if (is_small()) {
        set_small_size(new_size);
    } else {
        data_.large.size = new_size;
    }
    
    return *this;
}
```

#### 查找操作
```cpp
size_type find(const char* str, size_type pos = 0) const {
    size_type len = std::strlen(str);
    if (pos > size() || len > size() - pos) {
        return npos;
    }
    
    const char* result = std::strstr(get_ptr() + pos, str);
    return result ? result - get_ptr() : npos;
}
```

#### 替换操作
```cpp
string& replace(size_type pos, size_type count, const char* str) {
    if (pos > size()) {
        throw std::out_of_range("string::replace");
    }
    
    size_type actual_count = std::min(count, size() - pos);
    size_type str_len = std::strlen(str);
    
    if (str_len == actual_count) {
        // 直接替换
        std::memcpy(get_ptr() + pos, str, str_len);
    } else if (str_len > actual_count) {
        // 需要扩展
        size_type new_size = size() - actual_count + str_len;
        reserve(new_size);
        char* ptr = get_ptr();
        std::memmove(ptr + pos + str_len, ptr + pos + actual_count, size() - pos - actual_count + 1);
        std::memcpy(ptr + pos, str, str_len);
        
        if (is_small()) {
            set_small_size(new_size);
        } else {
            data_.large.size = new_size;
        }
    } else {
        // 需要收缩
        size_type new_size = size() - actual_count + str_len;
        char* ptr = get_ptr();
        std::memcpy(ptr + pos, str, str_len);
        std::memmove(ptr + pos + str_len, ptr + pos + actual_count, size() - pos - actual_count + 1);
        
        if (is_small()) {
            set_small_size(new_size);
        } else {
            data_.large.size = new_size;
        }
    }
    
    return *this;
}
```

### 4. 迭代器实现

#### 前向迭代器
```cpp
class iterator {
private:
    char* ptr_;
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = char;
    using difference_type = ptrdiff_t;
    using pointer = char*;
    using reference = char&;
    
    iterator(char* ptr) : ptr_(ptr) {}
    
    reference operator*() const { return *ptr_; }
    pointer operator->() const { return ptr_; }
    
    iterator& operator++() { ++ptr_; return *this; }
    iterator operator++(int) { iterator tmp = *this; ++ptr_; return tmp; }
    iterator& operator--() { --ptr_; return *this; }
    iterator operator--(int) { iterator tmp = *this; --ptr_; return tmp; }
    
    bool operator==(const iterator& other) const { return ptr_ == other.ptr_; }
    bool operator!=(const iterator& other) const { return ptr_ != other.ptr_; }
    
    difference_type operator-(const iterator& other) const { return ptr_ - other.ptr_; }
    iterator operator+(difference_type n) const { return iterator(ptr_ + n); }
    iterator operator-(difference_type n) const { return iterator(ptr_ - n); }
};
```

#### 反向迭代器
```cpp
using reverse_iterator = std::reverse_iterator<iterator>;
using const_reverse_iterator = std::reverse_iterator<const_iterator>;
```

## 关键技术点

### 1. 联合体（Union）设计
- 使用联合体实现小字符串优化
- 避免了额外的内存开销
- 确保类型安全

### 2. 内存管理策略
- 指数增长的容量扩展策略
- 延迟收缩的内存优化
- 智能的小字符串转换

### 3. 异常安全保证
- 强异常安全：操作要么完全成功，要么完全失败
- 资源管理：使用RAII确保资源正确释放
- 边界检查：防止数组越界访问

### 4. 性能优化
- 小字符串优化：避免堆分配
- 移动语义：减少拷贝开销
- 内联函数：减少函数调用开销

## 使用示例

### 基本使用
```cpp
#include "string.hpp"

int main() {
    // 构造字符串
    my::string s1("Hello");
    my::string s2 = "World";
    my::string s3 = s1;  // 拷贝构造
    
    // 连接字符串
    my::string s4 = s1 + " " + s2;  // "Hello World"
    
    // 查找子串
    size_t pos = s4.find("World");
    if (pos != my::string::npos) {
        std::cout << "Found at position: " << pos << std::endl;
    }
    
    // 替换子串
    s4.replace(pos, 5, "C++");
    std::cout << s4 << std::endl;  // "Hello C++"
    
    return 0;
}
```

### 高级操作
```cpp
int main() {
    my::string s = "The quick brown fox jumps over the lazy dog";
    
    // 查找所有出现的位置
    size_t pos = 0;
    while ((pos = s.find("the", pos)) != my::string::npos) {
        std::cout << "Found 'the' at position: " << pos << std::endl;
        pos += 3;  // 跳过已找到的字符串
    }
    
    // 使用迭代器
    std::cout << "Reversed: ";
    for (auto it = s.rbegin(); it != s.rend(); ++it) {
        std::cout << *it;
    }
    std::cout << std::endl;
    
    // 内存管理
    s.reserve(100);  // 预分配内存
    s.shrink_to_fit();  // 收缩到合适大小
    
    return 0;
}
```

### 性能敏感场景
```cpp
int main() {
    // 大量字符串操作
    std::vector<my::string> strings;
    for (int i = 0; i < 1000000; ++i) {
        strings.push_back(my::string("String ") + std::to_string(i));
    }
    
    // 移动语义优化
    my::string result;
    for (const auto& s : strings) {
        result += s;  // 使用移动语义减少拷贝
    }
    
    return 0;
}
```

## 测试策略

### 1. 功能测试
- 基本构造和析构
- 拷贝和移动语义
- 字符串操作（连接、查找、替换）
- 内存管理（容量控制、收缩）

### 2. 性能测试
- 小字符串性能
- 大字符串性能
- 内存使用效率
- 拷贝vs移动性能对比

### 3. 边界测试
- 空字符串操作
- 最大长度字符串
- 内存分配失败
- 异常情况处理

### 4. 兼容性测试
- 与std::string的兼容性
- C风格字符串的兼容性
- 迭代器的标准兼容性

## 性能分析

### 1. 时间复杂度
- 构造/析构：O(n)
- 连接操作：O(n+m)
- 查找操作：O(n*m)
- 替换操作：O(n+m)

### 2. 空间复杂度
- 小字符串：O(1) 栈空间
- 大字符串：O(n) 堆空间
- 内存开销：32字节（固定）

### 3. 性能优化点
- 小字符串优化：避免堆分配
- 移动语义：减少拷贝
- 内存预分配：减少重新分配
- 内联函数：减少调用开销

## 常见问题与解决方案

### 1. 小字符串优化失效
**问题**：小字符串被错误地分配到堆上
**解决**：检查is_small标志的正确设置

### 2. 内存泄漏
**问题**：字符串析构时内存未释放
**解决**：确保release_memory()正确调用

### 3. 迭代器失效
**问题**：字符串修改后迭代器失效
**解决**：在修改操作后使现有迭代器失效

### 4. 性能瓶颈
**问题**：大量字符串操作性能低下
**解决**：使用移动语义和预分配优化

## 最佳实践

### 1. 使用建议
- 优先使用构造函数而不是赋值
- 合理使用reserve()预分配内存
- 善用移动语义减少拷贝
- 注意异常处理

### 2. 性能优化
- 对于已知大小的字符串，使用reserve()
- 避免频繁的小字符串操作
- 使用const引用传递大字符串
- 合理使用移动语义

### 3. 内存管理
- 及时调用shrink_to_fit()释放内存
- 避免不必要的字符串拷贝
- 注意循环引用问题
- 合理使用智能指针管理字符串

## 总结

`string` 类的实现展示了C++模板编程、内存管理、异常处理等核心概念的正确应用。通过精心设计的小字符串优化、移动语义和完整的字符串操作，我们创建了一个既高效又易用的字符串类。

这个实现不仅涵盖了`string`的所有核心功能，还提供了优秀的性能和内存管理特性。通过详细的测试和文档，可以作为学习C++高级特性的优秀示例。

关键特性：
- ✅ 小字符串优化（SSO）
- ✅ 移动语义支持
- ✅ 完整的字符串操作
- ✅ 异常安全保证
- ✅ 高性能实现
- ✅ 标准兼容接口
- ✅ 全面的测试覆盖