#ifndef MY_SHARED_PTR_HPP
#define MY_SHARED_PTR_HPP

#include <iostream>
#include <atomic>
#include <stdexcept>
#include <functional>
#include <mutex>
#include <memory>

namespace my {

// 控制块结构，管理引用计数
template <typename T>
class control_block {
public:
    std::atomic<size_t> shared_count;
    std::atomic<size_t> weak_count;
    T* ptr;
    std::function<void(T*)> deleter;

    control_block(T* p, std::function<void(T*)> d = [](T* ptr) { delete ptr; })
        : shared_count(1), weak_count(0), ptr(p), deleter(d) {}

    ~control_block() {
        // 在decrement_shared中已经处理了删除
    }

    void increment_shared() {
        shared_count.fetch_add(1, std::memory_order_relaxed);
    }

    void decrement_shared() {
        if (shared_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            if (ptr && deleter) {
                deleter(ptr);
                ptr = nullptr;
            }
            // 当shared_count变为0时，不需要立即调用decrement_weak
            // weak_ptr仍然可以访问控制块
        }
    }

    void increment_weak() {
        weak_count.fetch_add(1, std::memory_order_relaxed);
    }

    void decrement_weak() {
        if (weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete this;
        }
    }

    size_t get_shared_count() const {
        return shared_count.load(std::memory_order_relaxed);
    }

    size_t get_weak_count() const {
        return weak_count.load(std::memory_order_relaxed);
    }
};

// weak_ptr前置声明
template <typename T>
class weak_ptr;

// shared_ptr实现
template <typename T>
class shared_ptr {
private:
    T* ptr_;
    control_block<T>* ctrl_block_;

    template <typename U>
    friend class shared_ptr;
    
    template <typename U>
    friend class weak_ptr;

    // 辅助函数：增加引用计数
    void increment_ref() {
        if (ctrl_block_) {
            ctrl_block_->increment_shared();
        }
    }

    // 辅助函数：减少引用计数
    void decrement_ref() {
        if (ctrl_block_) {
            ctrl_block_->decrement_shared();
            ptr_ = nullptr;
            ctrl_block_ = nullptr;
        }
    }

public:
    // 类型定义
    using element_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    // 构造函数
    constexpr shared_ptr() noexcept : ptr_(nullptr), ctrl_block_(nullptr) {}

    constexpr shared_ptr(std::nullptr_t) noexcept : ptr_(nullptr), ctrl_block_(nullptr) {}

    explicit shared_ptr(T* ptr) : ptr_(ptr) {
        if (ptr) {
            ctrl_block_ = new control_block<T>(ptr);
        } else {
            ctrl_block_ = nullptr;
        }
    }

    template <typename U>
    explicit shared_ptr(U* ptr) : ptr_(ptr) {
        if (ptr) {
            ctrl_block_ = new control_block<T>(ptr);
        } else {
            ctrl_block_ = nullptr;
        }
    }

    // 带自定义删除器的构造函数
    template <typename Deleter>
    shared_ptr(T* ptr, Deleter d) : ptr_(ptr) {
        if (ptr) {
            ctrl_block_ = new control_block<T>(ptr, d);
        } else {
            ctrl_block_ = nullptr;
        }
    }

    // 拷贝构造函数
    shared_ptr(const shared_ptr& other) noexcept : ptr_(other.ptr_), ctrl_block_(other.ctrl_block_) {
        increment_ref();
    }

    template <typename U>
    shared_ptr(const shared_ptr<U>& other) noexcept : ptr_(other.ptr_), ctrl_block_(other.ctrl_block_) {
        increment_ref();
    }

    // 移动构造函数
    shared_ptr(shared_ptr&& other) noexcept : ptr_(other.ptr_), ctrl_block_(other.ctrl_block_) {
        other.ptr_ = nullptr;
        other.ctrl_block_ = nullptr;
    }

    template <typename U>
    shared_ptr(shared_ptr<U>&& other) noexcept : ptr_(other.ptr_), ctrl_block_(other.ctrl_block_) {
        other.ptr_ = nullptr;
        other.ctrl_block_ = nullptr;
    }

    // 从weak_ptr构造
    explicit shared_ptr(const weak_ptr<T>& other);

    // 析构函数
    ~shared_ptr() {
        decrement_ref();
    }

    // 拷贝赋值运算符
    shared_ptr& operator=(const shared_ptr& other) noexcept {
        if (this != &other) {
            decrement_ref();
            ptr_ = other.ptr_;
            ctrl_block_ = other.ctrl_block_;
            increment_ref();
        }
        return *this;
    }

    template <typename U>
    shared_ptr& operator=(const shared_ptr<U>& other) noexcept {
        if (this != reinterpret_cast<const shared_ptr*>(&other)) {
            decrement_ref();
            ptr_ = other.ptr_;
            ctrl_block_ = other.ctrl_block_;
            increment_ref();
        }
        return *this;
    }

    // 移动赋值运算符
    shared_ptr& operator=(shared_ptr&& other) noexcept {
        if (this != &other) {
            decrement_ref();
            ptr_ = other.ptr_;
            ctrl_block_ = other.ctrl_block_;
            other.ptr_ = nullptr;
            other.ctrl_block_ = nullptr;
        }
        return *this;
    }

    template <typename U>
    shared_ptr& operator=(shared_ptr<U>&& other) noexcept {
        if (this != reinterpret_cast<shared_ptr*>(&other)) {
            decrement_ref();
            ptr_ = other.ptr_;
            ctrl_block_ = other.ctrl_block_;
            other.ptr_ = nullptr;
            other.ctrl_block_ = nullptr;
        }
        return *this;
    }

    // 重置方法
    void reset() noexcept {
        decrement_ref();
        ptr_ = nullptr;
        ctrl_block_ = nullptr;
    }

    void reset(T* ptr) {
        if (ptr_ != ptr) {
            decrement_ref();
            ptr_ = ptr;
            if (ptr) {
                ctrl_block_ = new control_block<T>(ptr);
            } else {
                ctrl_block_ = nullptr;
            }
        }
    }

    template <typename Deleter>
    void reset(T* ptr, Deleter d) {
        if (ptr_ != ptr) {
            decrement_ref();
            ptr_ = ptr;
            if (ptr) {
                ctrl_block_ = new control_block<T>(ptr, d);
            } else {
                ctrl_block_ = nullptr;
            }
        }
    }

    // 交换方法
    void swap(shared_ptr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(ctrl_block_, other.ctrl_block_);
    }

    // 获取原始指针
    T* get() const noexcept {
        return ptr_;
    }

    // 解引用操作符
    T& operator*() const {
        if (!ptr_) {
            throw std::runtime_error("shared_ptr: null pointer dereference");
        }
        return *ptr_;
    }

    T* operator->() const {
        if (!ptr_) {
            throw std::runtime_error("shared_ptr: null pointer dereference");
        }
        return ptr_;
    }

    // 获取引用计数
    size_t use_count() const noexcept {
        return ctrl_block_ ? ctrl_block_->get_shared_count() : 0;
    }

    // 检查是否唯一
    bool unique() const noexcept {
        return use_count() == 1;
    }

    // 布尔操作符
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    // 获取所有权信息
    // owner_before未实现，暂时省略
};

// weak_ptr实现
template <typename T>
class weak_ptr {
private:
    T* ptr_;
    control_block<T>* ctrl_block_;

    template <typename U>
    friend class weak_ptr;
    
    template <typename U>
    friend class shared_ptr;

    void increment_weak() {
        if (ctrl_block_) {
            ctrl_block_->increment_weak();
        }
    }

    void decrement_weak() {
        if (ctrl_block_) {
            ctrl_block_->decrement_weak();
            ptr_ = nullptr;
            ctrl_block_ = nullptr;
        }
    }

public:
    // 类型定义
    using element_type = T;

    // 构造函数
    constexpr weak_ptr() noexcept : ptr_(nullptr), ctrl_block_(nullptr) {}

    weak_ptr(const weak_ptr& other) noexcept : ptr_(other.ptr_), ctrl_block_(other.ctrl_block_) {
        increment_weak();
    }

    template <typename U>
    weak_ptr(const weak_ptr<U>& other) noexcept : ptr_(other.ptr_), ctrl_block_(other.ctrl_block_) {
        increment_weak();
    }

    weak_ptr(const shared_ptr<T>& other) noexcept : ptr_(other.ptr_), ctrl_block_(other.ctrl_block_) {
        increment_weak();
    }

    template <typename U>
    weak_ptr(const shared_ptr<U>& other) noexcept : ptr_(other.ptr_), ctrl_block_(other.ctrl_block_) {
        increment_weak();
    }

    weak_ptr(weak_ptr&& other) noexcept : ptr_(other.ptr_), ctrl_block_(other.ctrl_block_) {
        other.ptr_ = nullptr;
        other.ctrl_block_ = nullptr;
    }

    template <typename U>
    weak_ptr(weak_ptr<U>&& other) noexcept : ptr_(other.ptr_), ctrl_block_(other.ctrl_block_) {
        other.ptr_ = nullptr;
        other.ctrl_block_ = nullptr;
    }

    // 析构函数
    ~weak_ptr() {
        decrement_weak();
    }

    // 赋值运算符
    weak_ptr& operator=(const weak_ptr& other) noexcept {
        if (this != &other) {
            decrement_weak();
            ptr_ = other.ptr_;
            ctrl_block_ = other.ctrl_block_;
            increment_weak();
        }
        return *this;
    }

    template <typename U>
    weak_ptr& operator=(const weak_ptr<U>& other) noexcept {
        if (this != reinterpret_cast<const weak_ptr*>(&other)) {
            decrement_weak();
            ptr_ = other.ptr_;
            ctrl_block_ = other.ctrl_block_;
            increment_weak();
        }
        return *this;
    }

    weak_ptr& operator=(const shared_ptr<T>& other) noexcept {
        decrement_weak();
        ptr_ = other.ptr_;
        ctrl_block_ = other.ctrl_block_;
        increment_weak();
        return *this;
    }

    template <typename U>
    weak_ptr& operator=(const shared_ptr<U>& other) noexcept {
        decrement_weak();
        ptr_ = other.ptr_;
        ctrl_block_ = other.ctrl_block_;
        increment_weak();
        return *this;
    }

    weak_ptr& operator=(weak_ptr&& other) noexcept {
        if (this != &other) {
            decrement_weak();
            ptr_ = other.ptr_;
            ctrl_block_ = other.ctrl_block_;
            other.ptr_ = nullptr;
            other.ctrl_block_ = nullptr;
        }
        return *this;
    }

    template <typename U>
    weak_ptr& operator=(weak_ptr<U>&& other) noexcept {
        if (this != reinterpret_cast<weak_ptr*>(&other)) {
            decrement_weak();
            ptr_ = other.ptr_;
            ctrl_block_ = other.ctrl_block_;
            other.ptr_ = nullptr;
            other.ctrl_block_ = nullptr;
        }
        return *this;
    }

    // 重置方法
    void reset() noexcept {
        decrement_weak();
        ptr_ = nullptr;
        ctrl_block_ = nullptr;
    }

    // 交换方法
    void swap(weak_ptr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(ctrl_block_, other.ctrl_block_);
    }

    // 获取引用计数
    size_t use_count() const noexcept {
        return ctrl_block_ ? ctrl_block_->get_shared_count() : 0;
    }

    // 检查是否过期
    bool expired() const noexcept {
        return use_count() == 0;
    }

    // 锁定方法
    shared_ptr<T> lock() const noexcept {
        if (expired() || !ctrl_block_) {
            return shared_ptr<T>();
        }
        return shared_ptr<T>(*this);
    }

    // 获取所有权信息
    // owner_before未实现，暂时省略
};

// shared_ptr从weak_ptr构造的实现
template <typename T>
shared_ptr<T>::shared_ptr(const weak_ptr<T>& other) : ptr_(nullptr), ctrl_block_(nullptr) {
    if (!other.expired() && other.ctrl_block_) {
        ptr_ = other.ptr_;
        ctrl_block_ = other.ctrl_block_;
        increment_ref();
    }
}

// make_shared函数
template <typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args) {
    T* ptr = new T(std::forward<Args>(args)...);
    return shared_ptr<T>(ptr);
}

// 比较运算符
template <typename T, typename U>
bool operator==(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) noexcept {
    return lhs.get() == rhs.get();
}

template <typename T>
bool operator==(const shared_ptr<T>& lhs, std::nullptr_t) noexcept {
    return lhs.get() == nullptr;
}

template <typename T>
bool operator==(std::nullptr_t, const shared_ptr<T>& rhs) noexcept {
    return nullptr == rhs.get();
}

template <typename T, typename U>
bool operator!=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) noexcept {
    return lhs.get() != rhs.get();
}

template <typename T>
bool operator!=(const shared_ptr<T>& lhs, std::nullptr_t) noexcept {
    return lhs.get() != nullptr;
}

template <typename T>
bool operator!=(std::nullptr_t, const shared_ptr<T>& rhs) noexcept {
    return nullptr != rhs.get();
}

template <typename T, typename U>
bool operator<(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) noexcept {
    return lhs.get() < rhs.get();
}

template <typename T>
bool operator<(const shared_ptr<T>& lhs, std::nullptr_t) noexcept {
    return lhs.get() < nullptr;
}

template <typename T>
bool operator<(std::nullptr_t, const shared_ptr<T>& rhs) noexcept {
    return nullptr < rhs.get();
}

template <typename T, typename U>
bool operator<=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) noexcept {
    return lhs.get() <= rhs.get();
}

template <typename T, typename U>
bool operator>(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) noexcept {
    return lhs.get() > rhs.get();
}

template <typename T, typename U>
bool operator>=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) noexcept {
    return lhs.get() >= rhs.get();
}

// 交换函数
template <typename T>
void swap(shared_ptr<T>& lhs, shared_ptr<T>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename T>
void swap(weak_ptr<T>& lhs, weak_ptr<T>& rhs) noexcept {
    lhs.swap(rhs);
}

// 输出流操作符
template <typename T>
std::ostream& operator<<(std::ostream& os, const shared_ptr<T>& ptr) {
    os << ptr.get();
    return os;
}

} // namespace my

// 哈希支持（在std命名空间中特化）
namespace std {
    template <typename T>
    struct hash<my::shared_ptr<T>> {
        size_t operator()(const my::shared_ptr<T>& ptr) const noexcept {
            return std::hash<T*>()(ptr.get());
        }
    };
}

#endif // MY_SHARED_PTR_HPP