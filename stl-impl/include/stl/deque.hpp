#pragma once

#include "allocator.hpp"
#include "iterator.hpp"
#include "functional.hpp"
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <memory>
#include <algorithm>
#include <type_traits>

namespace stl {

// deque异常类
class deque_exception : public std::runtime_error {
public:
    explicit deque_exception(const std::string& msg) : std::runtime_error(msg) {}
};

// 简化的deque实现 - 使用双向链表节点结构
template<typename T, typename Allocator = allocator<T>>
class deque {
private:
    // 节点结构
    struct Node {
        T data;
        Node* prev;
        Node* next;
        
        Node(const T& val, Node* p = nullptr, Node* n = nullptr)
            : data(val), prev(p), next(n) {}
        
        Node(T&& val, Node* p = nullptr, Node* n = nullptr)
            : data(std::move(val)), prev(p), next(n) {}
        
        template<typename... Args>
        Node(Args&&... args, Node* p = nullptr, Node* n = nullptr)
            : data(std::forward<Args>(args)...), prev(p), next(n) {}
    };
    
    using node_allocator = typename Allocator::template rebind<Node>::other;
    using node_pointer = Node*;
    
public:
    // 类型定义
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    
    // 迭代器实现
    class deque_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        deque_iterator() : node_(nullptr) {}
        
        explicit deque_iterator(node_pointer node) : node_(node) {}
        
        reference operator*() const { return node_->data; }
        pointer operator->() const { return &node_->data; }
        
        deque_iterator& operator++() {
            node_ = node_->next;
            return *this;
        }
        
        deque_iterator operator++(int) {
            deque_iterator tmp = *this;
            ++*this;
            return tmp;
        }
        
        deque_iterator& operator--() {
            node_ = node_->prev;
            return *this;
        }
        
        deque_iterator operator--(int) {
            deque_iterator tmp = *this;
            --*this;
            return tmp;
        }
        
        bool operator==(const deque_iterator& other) const {
            return node_ == other.node_;
        }
        
        bool operator!=(const deque_iterator& other) const {
            return node_ != other.node_;
        }
        
        node_pointer node() const { return node_; }
        
    private:
        node_pointer node_;
        
        friend class deque<T, Allocator>;
    };
    
    // 常量迭代器实现
    class const_deque_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;
        
        const_deque_iterator() : node_(nullptr) {}
        
        explicit const_deque_iterator(node_pointer node) : node_(node) {}
        
        // 从非常量迭代器转换
        const_deque_iterator(const deque_iterator& other) : node_(other.node()) {}
        
        reference operator*() const { return node_->data; }
        pointer operator->() const { return &node_->data; }
        
        const_deque_iterator& operator++() {
            node_ = node_->next;
            return *this;
        }
        
        const_deque_iterator operator++(int) {
            const_deque_iterator tmp = *this;
            ++*this;
            return tmp;
        }
        
        const_deque_iterator& operator--() {
            node_ = node_->prev;
            return *this;
        }
        
        const_deque_iterator operator--(int) {
            const_deque_iterator tmp = *this;
            --*this;
            return tmp;
        }
        
        bool operator==(const const_deque_iterator& other) const {
            return node_ == other.node_;
        }
        
        bool operator!=(const const_deque_iterator& other) const {
            return node_ != other.node_;
        }
        
        node_pointer node() const { return node_; }
        
    private:
        node_pointer node_;
        
        friend class deque<T, Allocator>;
    };
    
    // 迭代器类型定义
    using iterator = deque_iterator;
    using const_iterator = const_deque_iterator;
    using reverse_iterator = stl::reverse_iterator<iterator>;
    using const_reverse_iterator = stl::reverse_iterator<const_iterator>;
    
    // 构造函数
    deque() : head_(nullptr), tail_(nullptr), size_(0) {}
    
    explicit deque(const Allocator& alloc) : alloc_(alloc), head_(nullptr), tail_(nullptr), size_(0) {}
    
    explicit deque(size_type count, const T& value = T(), const Allocator& alloc = Allocator())
        : alloc_(alloc), head_(nullptr), tail_(nullptr), size_(0) {
        for (size_type i = 0; i < count; ++i) {
            push_back(value);
        }
    }
    
    template<typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
    deque(InputIt first, InputIt last, const Allocator& alloc = Allocator())
        : alloc_(alloc), head_(nullptr), tail_(nullptr), size_(0) {
        for (auto it = first; it != last; ++it) {
            push_back(*it);
        }
    }
    
    deque(const deque& other)
        : alloc_(other.alloc_), head_(nullptr), tail_(nullptr), size_(0) {
        for (auto it = other.begin(); it != other.end(); ++it) {
            push_back(*it);
        }
    }
    
    deque(deque&& other) noexcept
        : alloc_(std::move(other.alloc_)), head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }
    
    deque(std::initializer_list<T> init, const Allocator& alloc = Allocator())
        : alloc_(alloc), head_(nullptr), tail_(nullptr), size_(0) {
        for (const auto& item : init) {
            push_back(item);
        }
    }
    
    // 析构函数
    ~deque() {
        clear();
    }
    
    // 赋值运算符
    deque& operator=(const deque& other) {
        if (this != &other) {
            clear();
            for (auto it = other.begin(); it != other.end(); ++it) {
                push_back(*it);
            }
        }
        return *this;
    }
    
    deque& operator=(deque&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            
            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    deque& operator=(std::initializer_list<T> init) {
        clear();
        for (const auto& item : init) {
            push_back(item);
        }
        return *this;
    }
    
    // 赋值函数
    template<typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
    void assign(InputIt first, InputIt last) {
        clear();
        for (auto it = first; it != last; ++it) {
            push_back(*it);
        }
    }
    
    void assign(size_type count, const T& value) {
        clear();
        for (size_type i = 0; i < count; ++i) {
            push_back(value);
        }
    }
    
    void assign(std::initializer_list<T> init) {
        clear();
        for (const auto& item : init) {
            push_back(item);
        }
    }
    
    // 获取分配器
    allocator_type get_allocator() const noexcept {
        return alloc_;
    }
    
    // 元素访问
    reference at(size_type pos) {
        if (pos >= size_) {
            throw deque_exception("deque::at: pos out of range");
        }
        return *std::next(begin(), pos);
    }
    
    const_reference at(size_type pos) const {
        if (pos >= size_) {
            throw deque_exception("deque::at: pos out of range");
        }
        return *std::next(begin(), pos);
    }
    
    reference operator[](size_type pos) {
        return *std::next(begin(), pos);
    }
    
    const_reference operator[](size_type pos) const {
        return *std::next(begin(), pos);
    }
    
    reference front() noexcept {
        return head_->data;
    }
    
    const_reference front() const noexcept {
        return head_->data;
    }
    
    reference back() noexcept {
        return tail_->data;
    }
    
    const_reference back() const noexcept {
        return tail_->data;
    }
    
    // 迭代器
    iterator begin() noexcept { return iterator(head_); }
    const_iterator begin() const noexcept { return const_iterator(head_); }
    const_iterator cbegin() const noexcept { return const_iterator(head_); }
    
    iterator end() noexcept { return iterator(nullptr); }
    const_iterator end() const noexcept { return const_iterator(nullptr); }
    const_iterator cend() const noexcept { return const_iterator(nullptr); }
    
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
    
    // 容量
    bool empty() const noexcept { return size_ == 0; }
    size_type size() const noexcept { return size_; }
    
    size_type max_size() const noexcept {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }
    
    void shrink_to_fit() {
        // 链表实现无需收缩
    }
    
    // 修改器
    void clear() noexcept {
        while (!empty()) {
            pop_front();
        }
    }
    
    iterator insert(const_iterator pos, const T& value) {
        if (pos == begin()) {
            push_front(value);
            return begin();
        } else if (pos == end()) {
            push_back(value);
            return --end();
        } else {
            node_pointer new_node = create_node(value);
            node_pointer current = pos.node();
            new_node->prev = current->prev;
            new_node->next = current;
            current->prev->next = new_node;
            current->prev = new_node;
            ++size_;
            return iterator(new_node);
        }
    }
    
    iterator insert(const_iterator pos, T&& value) {
        if (pos == begin()) {
            push_front(std::move(value));
            return begin();
        } else if (pos == end()) {
            push_back(std::move(value));
            return --end();
        } else {
            node_pointer new_node = create_node(std::move(value));
            node_pointer current = pos.node();
            new_node->prev = current->prev;
            new_node->next = current;
            current->prev->next = new_node;
            current->prev = new_node;
            ++size_;
            return iterator(new_node);
        }
    }
    
    iterator insert(const_iterator pos, size_type count, const T& value) {
        iterator result;
        if (pos == begin()) {
            for (size_type i = 0; i < count; ++i) {
                push_front(value);
            }
            result = begin();
        } else if (pos == end()) {
            result = end();
            for (size_type i = 0; i < count; ++i) {
                push_back(value);
            }
            std::advance(result, -static_cast<difference_type>(count));
        } else {
            result = iterator(pos.node());
            for (size_type i = 0; i < count; ++i) {
                result = insert(result, value);
                ++result;
            }
            std::advance(result, -static_cast<difference_type>(count));
        }
        return result;
    }
    
    template<typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        iterator result;
        if (pos == begin()) {
            for (auto it = last; it != first;) {
                --it;
                push_front(*it);
            }
            result = begin();
        } else if (pos == end()) {
            result = end();
            for (auto it = first; it != last; ++it) {
                push_back(*it);
            }
            std::advance(result, -std::distance(first, last));
        } else {
            result = iterator(pos.node());
            for (auto it = first; it != last; ++it) {
                result = insert(result, *it);
                ++result;
            }
            std::advance(result, -std::distance(first, last));
        }
        return result;
    }
    
    iterator insert(const_iterator pos, std::initializer_list<T> init) {
        return insert(pos, init.begin(), init.end());
    }
    
    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        if (pos == begin()) {
            emplace_front(std::forward<Args>(args)...);
            return begin();
        } else if (pos == end()) {
            emplace_back(std::forward<Args>(args)...);
            return --end();
        } else {
            node_pointer new_node = create_node(std::forward<Args>(args)...);
            node_pointer current = pos.node();
            new_node->prev = current->prev;
            new_node->next = current;
            current->prev->next = new_node;
            current->prev = new_node;
            ++size_;
            return iterator(new_node);
        }
    }
    
    iterator erase(const_iterator pos) {
        if (pos == begin()) {
            pop_front();
            return begin();
        } else if (pos == end()) {
            return end();
        } else {
            node_pointer current = pos.node();
            iterator result(current->next);
            current->prev->next = current->next;
            current->next->prev = current->prev;
            destroy_node(current);
            --size_;
            return result;
        }
    }
    
    iterator erase(const_iterator first, const_iterator last) {
        while (first != last) {
            first = erase(first);
        }
        return iterator(last.node());
    }
    
    void push_front(const T& value) {
        node_pointer new_node = create_node(value);
        if (empty()) {
            head_ = tail_ = new_node;
        } else {
            new_node->next = head_;
            head_->prev = new_node;
            head_ = new_node;
        }
        ++size_;
    }
    
    void push_front(T&& value) {
        node_pointer new_node = create_node(std::move(value));
        if (empty()) {
            head_ = tail_ = new_node;
        } else {
            new_node->next = head_;
            head_->prev = new_node;
            head_ = new_node;
        }
        ++size_;
    }
    
    template<typename... Args>
    reference emplace_front(Args&&... args) {
        node_pointer new_node = create_node(std::forward<Args>(args)...);
        if (empty()) {
            head_ = tail_ = new_node;
        } else {
            new_node->next = head_;
            head_->prev = new_node;
            head_ = new_node;
        }
        ++size_;
        return front();
    }
    
    void push_back(const T& value) {
        node_pointer new_node = create_node(value);
        if (empty()) {
            head_ = tail_ = new_node;
        } else {
            tail_->next = new_node;
            new_node->prev = tail_;
            tail_ = new_node;
        }
        ++size_;
    }
    
    void push_back(T&& value) {
        node_pointer new_node = create_node(std::move(value));
        if (empty()) {
            head_ = tail_ = new_node;
        } else {
            tail_->next = new_node;
            new_node->prev = tail_;
            tail_ = new_node;
        }
        ++size_;
    }
    
    template<typename... Args>
    reference emplace_back(Args&&... args) {
        node_pointer new_node = create_node(std::forward<Args>(args)...);
        if (empty()) {
            head_ = tail_ = new_node;
        } else {
            tail_->next = new_node;
            new_node->prev = tail_;
            tail_ = new_node;
        }
        ++size_;
        return back();
    }
    
    void pop_front() {
        if (!empty()) {
            node_pointer old_head = head_;
            head_ = head_->next;
            if (head_) {
                head_->prev = nullptr;
            } else {
                tail_ = nullptr;
            }
            destroy_node(old_head);
            --size_;
        }
    }
    
    void pop_back() {
        if (!empty()) {
            node_pointer old_tail = tail_;
            tail_ = tail_->prev;
            if (tail_) {
                tail_->next = nullptr;
            } else {
                head_ = nullptr;
            }
            destroy_node(old_tail);
            --size_;
        }
    }
    
    void resize(size_type new_size) {
        while (size_ > new_size) {
            pop_back();
        }
        while (size_ < new_size) {
            push_back(T());
        }
    }
    
    void resize(size_type new_size, const T& value) {
        while (size_ > new_size) {
            pop_back();
        }
        while (size_ < new_size) {
            push_back(value);
        }
    }
    
    void swap(deque& other) noexcept {
        using std::swap;
        swap(head_, other.head_);
        swap(tail_, other.tail_);
        swap(size_, other.size_);
        swap(alloc_, other.alloc_);
    }

private:
    node_allocator alloc_;
    node_pointer head_;
    node_pointer tail_;
    size_type size_;
    
    // 节点管理
    template<typename... Args>
    node_pointer create_node(Args&&... args) {
        node_pointer node = alloc_.allocate(1);
        try {
            alloc_.construct(node, std::forward<Args>(args)...);
        } catch (...) {
            alloc_.deallocate(node, 1);
            throw;
        }
        return node;
    }
    
    void destroy_node(node_pointer node) {
        alloc_.destroy(node);
        alloc_.deallocate(node, 1);
    }
};

// deque比较运算符
template<typename T, typename Allocator>
bool operator==(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, typename Allocator>
bool operator!=(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
    return !(lhs == rhs);
}

template<typename T, typename Allocator>
bool operator<(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T, typename Allocator>
bool operator<=(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
    return !(rhs < lhs);
}

template<typename T, typename Allocator>
bool operator>(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
    return rhs < lhs;
}

template<typename T, typename Allocator>
bool operator>=(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
    return !(lhs < rhs);
}

// deque特化的swap函数
template<typename T, typename Allocator>
void swap(deque<T, Allocator>& lhs, deque<T, Allocator>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace stl