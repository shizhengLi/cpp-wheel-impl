#ifndef STL_PRIORITY_QUEUE_HPP
#define STL_PRIORITY_QUEUE_HPP

#include "../vector.hpp"
#include "../functional.hpp"
#include "../algorithm/heap.hpp"
#include <utility>
#include <algorithm>
#include <type_traits>

namespace stl {

template <typename T, typename Container = vector<T>, typename Compare = less<typename Container::value_type>>
class priority_queue {
public:
    using container_type = Container;
    using value_compare = Compare;
    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;

    // 构造函数
    priority_queue() : c_(), comp_() {}
    
    explicit priority_queue(const Compare& comp) : c_(), comp_(comp) {}
    
    explicit priority_queue(const Compare& comp, const Container& cont) : c_(cont), comp_(comp) {
        make_heap(c_.begin(), c_.end(), comp_);
    }
    
    explicit priority_queue(const Compare& comp, Container&& cont) : c_(std::move(cont)), comp_(comp) {
        make_heap(c_.begin(), c_.end(), comp_);
    }
    
    priority_queue(const priority_queue& other) : c_(other.c_), comp_(other.comp_) {}
    
    priority_queue(priority_queue&& other) noexcept : c_(std::move(other.c_)), comp_(std::move(other.comp_)) {}
    
    template <typename InputIt>
    priority_queue(InputIt first, InputIt last, const Compare& comp = Compare()) : c_(first, last), comp_(comp) {
        stl::make_heap(c_.begin(), c_.end(), comp_);
    }
    
    template <typename InputIt>
    priority_queue(InputIt first, InputIt last, const Compare& comp, const Container& cont) : c_(cont), comp_(comp) {
        c_.insert(c_.end(), first, last);
        stl::make_heap(c_.begin(), c_.end(), comp_);
    }
    
    template <typename InputIt>
    priority_queue(InputIt first, InputIt last, const Compare& comp, Container&& cont) : c_(std::move(cont)), comp_(comp) {
        c_.insert(c_.end(), first, last);
        stl::make_heap(c_.begin(), c_.end(), comp_);
    }

    // 赋值运算符
    priority_queue& operator=(const priority_queue& other) {
        c_ = other.c_;
        comp_ = other.comp_;
        return *this;
    }
    
    priority_queue& operator=(priority_queue&& other) noexcept {
        c_ = std::move(other.c_);
        comp_ = std::move(other.comp_);
        return *this;
    }

    // 元素访问
    const_reference top() const {
        return c_.front();
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
        stl::push_heap(c_.begin(), c_.end(), comp_);
    }
    
    void push(value_type&& value) {
        c_.push_back(std::move(value));
        stl::push_heap(c_.begin(), c_.end(), comp_);
    }
    
    template <typename... Args>
    void emplace(Args&&... args) {
        c_.emplace_back(std::forward<Args>(args)...);
        stl::push_heap(c_.begin(), c_.end(), comp_);
    }
    
    void pop() {
        stl::pop_heap(c_.begin(), c_.end(), comp_);
        c_.pop_back();
    }
    
    void swap(priority_queue& other) noexcept {
        using std::swap;
        swap(c_, other.c_);
        swap(comp_, other.comp_);
    }

    // 获取底层容器
    container_type& get_container() noexcept {
        return c_;
    }
    
    const container_type& get_container() const noexcept {
        return c_;
    }
    
    // 获取比较器
    value_compare& get_comparator() noexcept {
        return comp_;
    }
    
    const value_compare& get_comparator() const noexcept {
        return comp_;
    }

protected:
    Container c_;
    Compare comp_;
};

// 比较操作
template <typename T, typename Container, typename Compare>
bool operator==(const priority_queue<T, Container, Compare>& lhs, const priority_queue<T, Container, Compare>& rhs) {
    return lhs.get_container() == rhs.get_container();
}

template <typename T, typename Container, typename Compare>
bool operator!=(const priority_queue<T, Container, Compare>& lhs, const priority_queue<T, Container, Compare>& rhs) {
    return !(lhs == rhs);
}

template <typename T, typename Container, typename Compare>
void swap(priority_queue<T, Container, Compare>& lhs, priority_queue<T, Container, Compare>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace stl

#endif // STL_PRIORITY_QUEUE_HPP