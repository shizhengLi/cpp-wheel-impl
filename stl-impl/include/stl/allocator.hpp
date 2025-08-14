#ifndef STL_ALLOCATOR_HPP
#define STL_ALLOCATOR_HPP

#include <cstddef>
#include <cstdlib>
#include <new>
#include <limits>
#include <type_traits>
#include <utility>
#include <iterator>
#include <cstdint>

namespace stl {

// 默认内存分配器
template <typename T>
class allocator {
public:
    // 类型定义
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    
    // rebind用于分配不同类型的内存
    template <typename U>
    struct rebind {
        using other = allocator<U>;
    };
    
    // 构造函数和析构函数
    allocator() noexcept = default;
    
    template <typename U>
    allocator(const allocator<U>&) noexcept {}
    
    ~allocator() noexcept = default;
    
    // 分配内存
    pointer allocate(size_type n, const void* hint = nullptr) {
        (void)hint; // 避免未使用参数警告
        
        if (n > max_size()) {
            throw std::bad_alloc();
        }
        
        if (n == 0) {
            return nullptr;
        }
        
        void* p = std::malloc(n * sizeof(T));
        if (p == nullptr) {
            throw std::bad_alloc();
        }
        
        return static_cast<pointer>(p);
    }
    
    // 释放内存
    void deallocate(pointer p, size_type n) noexcept {
        if (p != nullptr) {
            std::free(p);
        }
        (void)n; // 避免未使用参数警告
    }
    
    // 构造对象
    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        ::new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }
    
    // 销毁对象
    template <typename U>
    void destroy(U* p) noexcept {
        if (p != nullptr) {
            p->~U();
        }
    }
    
    // 获取对象地址
    pointer address(reference x) const noexcept {
        return std::addressof(x);
    }
    
    const_pointer address(const_reference x) const noexcept {
        return std::addressof(x);
    }
    
    // 最大可分配大小
    size_type max_size() const noexcept {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }
};

// 分配器相等比较
template <typename T, typename U>
bool operator==(const allocator<T>&, const allocator<U>&) noexcept {
    return true;
}

template <typename T, typename U>
bool operator!=(const allocator<T>&, const allocator<U>&) noexcept {
    return false;
}

// 特化void类型的分配器
template <>
class allocator<void> {
public:
    using value_type = void;
    using pointer = void*;
    using const_pointer = const void*;
    
    template <typename U>
    struct rebind {
        using other = allocator<U>;
    };
};

// 全局分配器类型别名
template <typename T>
using default_allocator = allocator<T>;

// 辅助函数：未初始化的拷贝
template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
    ForwardIterator current = result;
    try {
        for (; first != last; ++first, ++current) {
            ::new(static_cast<void*>(std::addressof(*current))) typename std::iterator_traits<ForwardIterator>::value_type(*first);
        }
        return current;
    } catch (...) {
        // 回滚已构造的对象
        for (; result != current; ++result) {
            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            result->~value_type();
        }
        throw;
    }
}

// 辅助函数：未初始化的填充
template <typename ForwardIterator, typename T>
void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value) {
    ForwardIterator current = first;
    try {
        for (; current != last; ++current) {
            ::new(static_cast<void*>(std::addressof(*current))) typename std::iterator_traits<ForwardIterator>::value_type(value);
        }
    } catch (...) {
        // 回滚已构造的对象
        for (; first != current; ++first) {
            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            first->~value_type();
        }
        throw;
    }
}

// 辅助函数：未初始化的填充n个
template <typename ForwardIterator, typename Size, typename T>
ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& value) {
    ForwardIterator current = first;
    try {
        for (; n > 0; --n, ++current) {
            ::new(static_cast<void*>(std::addressof(*current))) typename std::iterator_traits<ForwardIterator>::value_type(value);
        }
        return current;
    } catch (...) {
        // 回滚已构造的对象
        for (; first != current; ++first) {
            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            first->~value_type();
        }
        throw;
    }
}

// 辅助函数：未初始化的默认构造
template <typename ForwardIterator>
ForwardIterator uninitialized_default_construct(ForwardIterator first, ForwardIterator last) {
    ForwardIterator current = first;
    try {
        for (; current != last; ++current) {
            ::new(static_cast<void*>(std::addressof(*current))) typename std::iterator_traits<ForwardIterator>::value_type();
        }
        return current;
    } catch (...) {
        // 回滚已构造的对象
        for (; first != current; ++first) {
            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            first->~value_type();
        }
        throw;
    }
}

// 辅助函数：未初始化的值构造
template <typename ForwardIterator>
ForwardIterator uninitialized_value_construct(ForwardIterator first, ForwardIterator last) {
    ForwardIterator current = first;
    try {
        for (; current != last; ++current) {
            ::new(static_cast<void*>(std::addressof(*current))) typename std::iterator_traits<ForwardIterator>::value_type();
        }
        return current;
    } catch (...) {
        // 回滚已构造的对象
        for (; first != current; ++first) {
            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            first->~value_type();
        }
        throw;
    }
}

// 辅助函数：未初始化的移动
template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_move(InputIterator first, InputIterator last, ForwardIterator result) {
    ForwardIterator current = result;
    try {
        for (; first != last; ++first, ++current) {
            ::new(static_cast<void*>(std::addressof(*current))) 
                typename std::iterator_traits<ForwardIterator>::value_type(std::move(*first));
        }
        return current;
    } catch (...) {
        // 回滚已构造的对象
        for (; result != current; ++result) {
            using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
            result->~value_type();
        }
        throw;
    }
}

// 辅助函数：销毁范围
template <typename ForwardIterator>
void destroy(ForwardIterator first, ForwardIterator last) noexcept {
    for (; first != last; ++first) {
        using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
        first->~value_type();
    }
}

// 辅助函数：销毁n个
template <typename ForwardIterator, typename Size>
ForwardIterator destroy_n(ForwardIterator first, Size n) noexcept {
    for (; n > 0; --n, ++first) {
        using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
        first->~value_type();
    }
    return first;
}

} // namespace stl

#endif // STL_ALLOCATOR_HPP