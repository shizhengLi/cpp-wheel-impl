#ifndef STL_QUEUE_HPP
#define STL_QUEUE_HPP

#include "../deque.hpp"
#include <utility>
#include <type_traits>

namespace stl {

template <typename T, typename Container = deque<T>>
class queue {
public:
    using container_type = Container;
    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;

    // 构造函数
    queue() : c_() {}
    
    explicit queue(const Container& cont) : c_(cont) {}
    
    explicit queue(Container&& cont) : c_(std::move(cont)) {}
    
    queue(const queue& other) : c_(other.c_) {}
    
    queue(queue&& other) noexcept : c_(std::move(other.c_)) {}
    
    template <typename Alloc, typename = std::void_t<typename Container::allocator_type>>
    explicit queue(const Alloc& alloc) : c_(alloc) {}
    
    template <typename Alloc, typename = std::void_t<typename Container::allocator_type>>
    queue(const Container& cont, const Alloc& alloc) : c_(cont, alloc) {}
    
    template <typename Alloc, typename = std::void_t<typename Container::allocator_type>>
    queue(Container&& cont, const Alloc& alloc) : c_(std::move(cont), alloc) {}
    
    template <typename Alloc, typename = std::void_t<typename Container::allocator_type>>
    queue(const queue& other, const Alloc& alloc) : c_(other.c_, alloc) {}
    
    template <typename Alloc, typename = std::void_t<typename Container::allocator_type>>
    queue(queue&& other, const Alloc& alloc) : c_(std::move(other.c_), alloc) {}

    // 赋值运算符
    queue& operator=(const queue& other) {
        c_ = other.c_;
        return *this;
    }
    
    queue& operator=(queue&& other) noexcept {
        c_ = std::move(other.c_);
        return *this;
    }

    // 元素访问
    reference front() {
        return c_.front();
    }
    
    const_reference front() const {
        return c_.front();
    }
    
    reference back() {
        return c_.back();
    }
    
    const_reference back() const {
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
        c_.pop_front();
    }
    
    void swap(queue& other) noexcept {
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
    friend bool operator==(const queue<T1, C1>&, const queue<T1, C1>&);
};

// 比较操作
template <typename T, typename Container>
bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return lhs.c_ == rhs.c_;
}

template <typename T, typename Container>
bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return !(lhs == rhs);
}

template <typename T, typename Container>
void swap(queue<T, Container>& lhs, queue<T, Container>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace stl

#endif // STL_QUEUE_HPP