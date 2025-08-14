#ifndef MY_UNIQUE_PTR_HPP
#define MY_UNIQUE_PTR_HPP

#include <iostream>
#include <utility>
#include <type_traits>
#include <stdexcept>

namespace my {

// 默认删除器
template<typename T>
struct DefaultDeleter {
    void operator()(T* ptr) const {
        delete ptr;
    }
};

// 数组特化的默认删除器
template<typename T>
struct DefaultDeleter<T[]> {
    void operator()(T* ptr) const {
        delete[] ptr;
    }
};

// 主模板
template<typename T, typename Deleter = DefaultDeleter<T>>
class unique_ptr {
private:
    T* ptr_;
    Deleter deleter_;

public:
    // 构造函数
    constexpr unique_ptr() noexcept : ptr_(nullptr) {}
    
    constexpr unique_ptr(std::nullptr_t) noexcept : ptr_(nullptr) {}
    
    explicit unique_ptr(T* ptr) noexcept : ptr_(ptr) {}
    
    unique_ptr(T* ptr, const Deleter& deleter) noexcept 
        : ptr_(ptr), deleter_(deleter) {}
    
    unique_ptr(T* ptr, Deleter&& deleter) noexcept 
        : ptr_(ptr), deleter_(std::move(deleter)) {}
    
    // 删除拷贝构造函数
    unique_ptr(const unique_ptr&) = delete;
    
    // 移动构造函数
    unique_ptr(unique_ptr&& other) noexcept 
        : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
        other.ptr_ = nullptr;
    }
    
    // 析构函数
    ~unique_ptr() {
        if (ptr_ != nullptr) {
            deleter_(ptr_);
        }
    }
    
    // 删除拷贝赋值运算符
    unique_ptr& operator=(const unique_ptr&) = delete;
    
    // 移动赋值运算符
    unique_ptr& operator=(unique_ptr&& other) noexcept {
        if (this != &other) {
            if (ptr_ != nullptr) {
                deleter_(ptr_);
            }
            ptr_ = other.ptr_;
            deleter_ = std::move(other.deleter_);
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    // 赋值nullptr
    unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }
    
    // 解引用运算符
    T& operator*() const {
        if (ptr_ == nullptr) {
            throw std::runtime_error("Dereferencing null unique_ptr");
        }
        return *ptr_;
    }
    
    // 箭头运算符
    T* operator->() const {
        if (ptr_ == nullptr) {
            throw std::runtime_error("Accessing null unique_ptr");
        }
        return ptr_;
    }
    
    // 获取原始指针
    T* get() const noexcept {
        return ptr_;
    }
    
    // 释放所有权
    T* release() noexcept {
        T* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }
    
    // 重置指针
    void reset(T* ptr = nullptr) noexcept {
        if (ptr_ != nullptr) {
            deleter_(ptr_);
        }
        ptr_ = ptr;
    }
    
    // 交换内容
    void swap(unique_ptr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(deleter_, other.deleter_);
    }
    
    // 转换为bool
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }
    
    // 禁止数组类型的主模板
    static_assert(!std::is_array_v<T>, "Use unique_ptr<T[]> for arrays");
};

// 数组特化
template<typename T, typename Deleter>
class unique_ptr<T[], Deleter> {
private:
    T* ptr_;
    Deleter deleter_;

public:
    // 构造函数
    constexpr unique_ptr() noexcept : ptr_(nullptr) {}
    
    constexpr unique_ptr(std::nullptr_t) noexcept : ptr_(nullptr) {}
    
    explicit unique_ptr(T* ptr) noexcept : ptr_(ptr) {}
    
    unique_ptr(T* ptr, const Deleter& deleter) noexcept 
        : ptr_(ptr), deleter_(deleter) {}
    
    unique_ptr(T* ptr, Deleter&& deleter) noexcept 
        : ptr_(ptr), deleter_(std::move(deleter)) {}
    
    // 删除拷贝构造函数
    unique_ptr(const unique_ptr&) = delete;
    
    // 移动构造函数
    unique_ptr(unique_ptr&& other) noexcept 
        : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
        other.ptr_ = nullptr;
    }
    
    // 析构函数
    ~unique_ptr() {
        if (ptr_ != nullptr) {
            deleter_(ptr_);
        }
    }
    
    // 删除拷贝赋值运算符
    unique_ptr& operator=(const unique_ptr&) = delete;
    
    // 移动赋值运算符
    unique_ptr& operator=(unique_ptr&& other) noexcept {
        if (this != &other) {
            if (ptr_ != nullptr) {
                deleter_(ptr_);
            }
            ptr_ = other.ptr_;
            deleter_ = std::move(other.deleter_);
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    // 赋值nullptr
    unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }
    
    // 数组下标运算符
    T& operator[](std::size_t index) const {
        if (ptr_ == nullptr) {
            throw std::runtime_error("Accessing null unique_ptr array");
        }
        return ptr_[index];
    }
    
    // 获取原始指针
    T* get() const noexcept {
        return ptr_;
    }
    
    // 释放所有权
    T* release() noexcept {
        T* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }
    
    // 重置指针
    void reset(T* ptr = nullptr) noexcept {
        if (ptr_ != nullptr) {
            deleter_(ptr_);
        }
        ptr_ = ptr;
    }
    
    // 交换内容
    void swap(unique_ptr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(deleter_, other.deleter_);
    }
    
    // 转换为bool
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }
};

// 交换函数
template<typename T, typename Deleter>
void swap(unique_ptr<T, Deleter>& lhs, unique_ptr<T, Deleter>& rhs) noexcept {
    lhs.swap(rhs);
}

// 比较运算符
template<typename T1, typename D1, typename T2, typename D2>
bool operator==(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return x.get() == y.get();
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator!=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return x.get() != y.get();
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator<(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return x.get() < y.get();
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator<=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return x.get() <= y.get();
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return x.get() > y.get();
}

template<typename T1, typename D1, typename T2, typename D2>
bool operator>=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return x.get() >= y.get();
}

// 与nullptr的比较
template<typename T, typename Deleter>
bool operator==(const unique_ptr<T, Deleter>& x, std::nullptr_t) {
    return x.get() == nullptr;
}

template<typename T, typename Deleter>
bool operator==(std::nullptr_t, const unique_ptr<T, Deleter>& x) {
    return x.get() == nullptr;
}

template<typename T, typename Deleter>
bool operator!=(const unique_ptr<T, Deleter>& x, std::nullptr_t) {
    return x.get() != nullptr;
}

template<typename T, typename Deleter>
bool operator!=(std::nullptr_t, const unique_ptr<T, Deleter>& x) {
    return x.get() != nullptr;
}

// make_unique 工厂函数
template<typename T, typename... Args>
std::enable_if_t<!std::is_array_v<T>, unique_ptr<T>>
make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// 数组版本的 make_unique
template<typename T>
std::enable_if_t<std::is_array_v<T>, unique_ptr<T>>
make_unique(std::size_t size) {
    using U = std::remove_extent_t<T>;
    return unique_ptr<T>(new U[size]());
}

} // namespace my

#endif // MY_UNIQUE_PTR_HPP