#ifndef STL_STACK_HPP
#define STL_STACK_HPP

#include "container/deque.hpp"
#include <utility>

namespace stl {

template <typename T, typename Container = deque<T>>
class stack {
public:
    using container_type = Container;
    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;

    // 构造函数
    stack() : c_() {}
    
    explicit stack(const Container& cont) : c_(cont) {}
    
    explicit stack(Container&& cont) : c_(std::move(cont)) {}
    
    stack(const stack& other) : c_(other.c_) {}
    
    stack(stack&& other) noexcept : c_(std::move(other.c_)) {}
    
    template <typename Alloc, typename = typename Container::allocator_type>
    explicit stack(const Alloc& alloc) : c_(alloc) {}
    
    template <typename Alloc, typename = typename Container::allocator_type>
    stack(const Container& cont, const Alloc& alloc) : c_(cont, alloc) {}
    
    template <typename Alloc, typename = typename Container::allocator_type>
    stack(Container&& cont, const Alloc& alloc) : c_(std::move(cont), alloc) {}
    
    template <typename Alloc, typename = typename Container::allocator_type>
    stack(const stack& other, const Alloc& alloc) : c_(other.c_, alloc) {}
    
    template <typename Alloc, typename = typename Container::allocator_type>
    stack(stack&& other, const Alloc& alloc) : c_(std::move(other.c_), alloc) {}

    // 赋值运算符
    stack& operator=(const stack& other) {
        c_ = other.c_;
        return *this;
    }
    
    stack& operator=(stack&& other) noexcept {
        c_ = std::move(other.c_);
        return *this;
    }

    // 元素访问
    reference top() {
        return c_.back();
    }
    
    const_reference top() const {
        return c_.back();
    }

    // 容量
    bool empty() const {
        return c_.empty();
    }
    
    size_type size() const {
        return c_.size();
    }

    // 修改器
    void push(const value_type& value) {
        c_.push_back(value);
    }
    
    void push(value_type&& value) {
        c_.push_back(std::move(value));
    }
    
    template <typename... Args>
    void emplace(Args&&... args) {
        c_.emplace_back(std::forward<Args>(args)...);
    }
    
    void pop() {
        c_.pop_back();
    }
    
    void swap(stack& other) noexcept {
        using std::swap;
        swap(c_, other.c_);
    }

    // 获取底层容器
    container_type& get_container() noexcept {
        return c_;
    }
    
    const container_type& get_container() const noexcept {
        return c_;
    }

protected:
    Container c_;

    // 友元函数用于比较操作
    template <typename T1, typename C1>
    friend bool operator==(const stack<T1, C1>&, const stack<T1, C1>&);
    
    template <typename T1, typename C1>
    friend bool operator<(const stack<T1, C1>&, const stack<T1, C1>&);
};

// 比较操作
template <typename T, typename Container>
bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return lhs.c_ == rhs.c_;
}

template <typename T, typename Container>
bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return !(lhs == rhs);
}

template <typename T, typename Container>
bool operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return lhs.c_ < rhs.c_;
}

template <typename T, typename Container>
bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return !(rhs < lhs);
}

template <typename T, typename Container>
bool operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return rhs < lhs;
}

template <typename T, typename Container>
bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return !(lhs < rhs);
}

template <typename T, typename Container>
void swap(stack<T, Container>& lhs, stack<T, Container>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace stl

#endif // STL_STACK_HPP