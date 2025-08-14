#pragma once

#include "allocator.hpp"
#include "iterator.hpp"
#include "functional.hpp"
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <memory>
#include <algorithm>

namespace stl {

// 简化版的allocator_traits实现
template<typename Alloc>
struct allocator_traits {
    using allocator_type = Alloc;
    using value_type = typename Alloc::value_type;
    using pointer = typename Alloc::pointer;
    using const_pointer = typename Alloc::const_pointer;
    using size_type = typename Alloc::size_type;
    using difference_type = typename Alloc::difference_type;
    
    template<typename U>
    using rebind_alloc = typename Alloc::template rebind<U>::other;
    
    static pointer allocate(Alloc& a, size_type n) {
        return a.allocate(n);
    }
    
    static void deallocate(Alloc& a, pointer p, size_type n) {
        a.deallocate(p, n);
    }
    
    template<typename U, typename... Args>
    static void construct(Alloc&, U* p, Args&&... args) {
        new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }
    
    template<typename U>
    static void destroy(Alloc&, U* p) {
        p->~U();
    }
    
    static size_type max_size(const Alloc& a) noexcept {
        return a.max_size();
    }
    
    static Alloc select_on_container_copy_construction(const Alloc& a) {
        return a;
    }
    
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::false_type;
};

// 特化版本以支持标准库
template<typename T>
struct allocator_traits<std::allocator<T>> {
    using allocator_type = std::allocator<T>;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    template<typename U>
    using rebind_alloc = std::allocator<U>;
    
    static pointer allocate(allocator_type& a, size_type n) {
        return a.allocate(n);
    }
    
    static void deallocate(allocator_type& a, pointer p, size_type n) {
        a.deallocate(p, n);
    }
    
    template<typename U, typename... Args>
    static void construct(allocator_type&, U* p, Args&&... args) {
        new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }
    
    template<typename U>
    static void destroy(allocator_type&, U* p) {
        p->~U();
    }
    
    static size_type max_size(const allocator_type& a) noexcept {
        return a.max_size();
    }
    
    static allocator_type select_on_container_copy_construction(const allocator_type& a) {
        return a;
    }
    
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::false_type;
};

// vector异常类
class vector_exception : public std::runtime_error {
public:
    explicit vector_exception(const std::string& msg) : std::runtime_error(msg) {}
};

// vector实现
template<typename T, typename Allocator = allocator<T>>
class vector {
public:
    // 类型定义
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = typename allocator_traits<Allocator>::size_type;
    using difference_type = typename allocator_traits<Allocator>::difference_type;
    using reference = T&;
    using const_reference = const T&;
    using pointer = typename allocator_traits<Allocator>::pointer;
    using const_pointer = typename allocator_traits<Allocator>::const_pointer;
    
    // 迭代器定义
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = stl::reverse_iterator<iterator>;
    using const_reverse_iterator = stl::reverse_iterator<const_iterator>;

    // 构造函数
    vector() noexcept(noexcept(Allocator())) : data_(nullptr), size_(0), capacity_(0) {}
    
    explicit vector(const Allocator& alloc) noexcept : data_(nullptr), size_(0), capacity_(0), alloc_(alloc) {}
    
    explicit vector(size_type count, const T& value, const Allocator& alloc = Allocator())
        : data_(nullptr), size_(0), capacity_(0), alloc_(alloc) {
        resize(count, value);
    }
    
    explicit vector(size_type count, const Allocator& alloc = Allocator())
        : data_(nullptr), size_(0), capacity_(0), alloc_(alloc) {
        resize(count);
    }
    
    template<typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
    vector(InputIt first, InputIt last, const Allocator& alloc = Allocator())
        : data_(nullptr), size_(0), capacity_(0), alloc_(alloc) {
        assign(first, last);
    }
    
    vector(const vector& other) 
        : data_(nullptr), size_(0), capacity_(0), alloc_(allocator_traits<Allocator>::select_on_container_copy_construction(other.alloc_)) {
        reserve(other.size_);
        for (size_type i = 0; i < other.size_; ++i) {
            push_back(other.data_[i]);
        }
    }
    
    vector(const vector& other, const Allocator& alloc)
        : data_(nullptr), size_(0), capacity_(0), alloc_(alloc) {
        reserve(other.size_);
        for (size_type i = 0; i < other.size_; ++i) {
            push_back(other.data_[i]);
        }
    }
    
    vector(vector&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_), alloc_(std::move(other.alloc_)) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    
    vector(vector&& other, const Allocator& alloc)
        : data_(nullptr), size_(0), capacity_(0), alloc_(alloc) {
        if (alloc_ == other.alloc_) {
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        } else {
            reserve(other.size_);
            for (size_type i = 0; i < other.size_; ++i) {
                push_back(std::move(other.data_[i]));
            }
        }
    }
    
    vector(std::initializer_list<T> init, const Allocator& alloc = Allocator())
        : data_(nullptr), size_(0), capacity_(0), alloc_(alloc) {
        reserve(init.size());
        for (const auto& item : init) {
            push_back(item);
        }
    }
    
    // 析构函数
    ~vector() {
        clear();
        if (data_) {
            alloc_.deallocate(data_, capacity_);
        }
    }
    
    // 赋值运算符
    vector& operator=(const vector& other) {
        if (this != &other) {
            if constexpr (allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
                alloc_ = other.alloc_;
            }
            assign(other.begin(), other.end());
        }
        return *this;
    }
    
    vector& operator=(vector&& other) noexcept {
        if (this != &other) {
            clear();
            if constexpr (allocator_traits<Allocator>::propagate_on_container_move_assignment::value) {
                alloc_ = std::move(other.alloc_);
                data_ = other.data_;
                size_ = other.size_;
                capacity_ = other.capacity_;
                other.data_ = nullptr;
                other.size_ = 0;
                other.capacity_ = 0;
            } else if (alloc_ == other.alloc_) {
                data_ = other.data_;
                size_ = other.size_;
                capacity_ = other.capacity_;
                other.data_ = nullptr;
                other.size_ = 0;
                other.capacity_ = 0;
            } else {
                assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
            }
        }
        return *this;
    }
    
    vector& operator=(std::initializer_list<T> init) {
        assign(init.begin(), init.end());
        return *this;
    }
    
    // 赋值函数
    template<typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
    void assign(InputIt first, InputIt last) {
        clear();
        reserve(std::distance(first, last));
        for (auto it = first; it != last; ++it) {
            push_back(*it);
        }
    }
    
    void assign(size_type count, const T& value) {
        clear();
        resize(count, value);
    }
    
    void assign(std::initializer_list<T> init) {
        assign(init.begin(), init.end());
    }
    
    // 获取分配器
    allocator_type get_allocator() const noexcept {
        return alloc_;
    }
    
    // 元素访问
    reference at(size_type pos) {
        if (pos >= size_) {
            throw vector_exception("vector::at: pos out of range");
        }
        return data_[pos];
    }
    
    const_reference at(size_type pos) const {
        if (pos >= size_) {
            throw vector_exception("vector::at: pos out of range");
        }
        return data_[pos];
    }
    
    reference operator[](size_type pos) noexcept {
        return data_[pos];
    }
    
    const_reference operator[](size_type pos) const noexcept {
        return data_[pos];
    }
    
    reference front() noexcept {
        return data_[0];
    }
    
    const_reference front() const noexcept {
        return data_[0];
    }
    
    reference back() noexcept {
        return data_[size_ - 1];
    }
    
    const_reference back() const noexcept {
        return data_[size_ - 1];
    }
    
    pointer data() noexcept {
        return data_;
    }
    
    const_pointer data() const noexcept {
        return data_;
    }
    
    // 迭代器
    iterator begin() noexcept {
        return data_;
    }
    
    const_iterator begin() const noexcept {
        return data_;
    }
    
    const_iterator cbegin() const noexcept {
        return data_;
    }
    
    iterator end() noexcept {
        return data_ + size_;
    }
    
    const_iterator end() const noexcept {
        return data_ + size_;
    }
    
    const_iterator cend() const noexcept {
        return data_ + size_;
    }
    
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    
    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    
    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }
    
    // 容量
    bool empty() const noexcept {
        return size_ == 0;
    }
    
    size_type size() const noexcept {
        return size_;
    }
    
    size_type max_size() const noexcept {
        return allocator_traits<Allocator>::max_size(alloc_);
    }
    
    void reserve(size_type new_cap) {
        if (new_cap > capacity_) {
            reallocate(new_cap);
        }
    }
    
    size_type capacity() const noexcept {
        return capacity_;
    }
    
    void shrink_to_fit() {
        if (size_ < capacity_) {
            reallocate(size_);
        }
    }
    
    // 修改器
    void clear() noexcept {
        for (size_type i = 0; i < size_; ++i) {
            allocator_traits<Allocator>::destroy(alloc_, data_ + i);
        }
        size_ = 0;
    }
    
    iterator insert(const_iterator pos, const T& value) {
        return emplace(pos, value);
    }
    
    iterator insert(const_iterator pos, T&& value) {
        return emplace(pos, std::move(value));
    }
    
    iterator insert(const_iterator pos, size_type count, const T& value) {
        size_type index = pos - begin();
        if (count == 0) {
            return begin() + index;
        }
        
        reserve(size_ + count);
        
        // 移动现有元素
        for (size_type i = size_; i > index; --i) {
            allocator_traits<Allocator>::construct(alloc_, data_ + i + count - 1, std::move(data_[i - 1]));
            allocator_traits<Allocator>::destroy(alloc_, data_ + i - 1);
        }
        
        // 插入新元素
        for (size_type i = 0; i < count; ++i) {
            allocator_traits<Allocator>::construct(alloc_, data_ + index + i, value);
        }
        
        size_ += count;
        return begin() + index;
    }
    
    template<typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        size_type index = pos - begin();
        size_type count = std::distance(first, last);
        
        if (count == 0) {
            return begin() + index;
        }
        
        reserve(size_ + count);
        
        // 移动现有元素
        for (size_type i = size_; i > index; --i) {
            allocator_traits<Allocator>::construct(alloc_, data_ + i + count - 1, std::move(data_[i - 1]));
            allocator_traits<Allocator>::destroy(alloc_, data_ + i - 1);
        }
        
        // 插入新元素
        for (auto it = first; it != last; ++it) {
            allocator_traits<Allocator>::construct(alloc_, data_ + index++, *it);
        }
        
        size_ += count;
        return begin() + (index - count);
    }
    
    iterator insert(const_iterator pos, std::initializer_list<T> init) {
        return insert(pos, init.begin(), init.end());
    }
    
    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        size_type index = pos - begin();
        
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        
        // 移动现有元素
        for (size_type i = size_; i > index; --i) {
            allocator_traits<Allocator>::construct(alloc_, data_ + i, std::move(data_[i - 1]));
            allocator_traits<Allocator>::destroy(alloc_, data_ + i - 1);
        }
        
        // 在指定位置构造新元素
        allocator_traits<Allocator>::construct(alloc_, data_ + index, std::forward<Args>(args)...);
        ++size_;
        
        return begin() + index;
    }
    
    iterator erase(const_iterator pos) {
        return erase(pos, pos + 1);
    }
    
    iterator erase(const_iterator first, const_iterator last) {
        size_type first_index = first - begin();
        size_type last_index = last - begin();
        size_type count = last_index - first_index;
        
        // 销毁要删除的元素
        for (size_type i = first_index; i < last_index; ++i) {
            allocator_traits<Allocator>::destroy(alloc_, data_ + i);
        }
        
        // 移动剩余元素
        for (size_type i = last_index; i < size_; ++i) {
            allocator_traits<Allocator>::construct(alloc_, data_ + i - count, std::move(data_[i]));
            allocator_traits<Allocator>::destroy(alloc_, data_ + i);
        }
        
        size_ -= count;
        return begin() + first_index;
    }
    
    void push_back(const T& value) {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        allocator_traits<Allocator>::construct(alloc_, data_ + size_, value);
        ++size_;
    }
    
    void push_back(T&& value) {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        allocator_traits<Allocator>::construct(alloc_, data_ + size_, std::move(value));
        ++size_;
    }
    
    template<typename... Args>
    reference emplace_back(Args&&... args) {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        allocator_traits<Allocator>::construct(alloc_, data_ + size_, std::forward<Args>(args)...);
        ++size_;
        return back();
    }
    
    void pop_back() {
        if (size_ > 0) {
            --size_;
            allocator_traits<Allocator>::destroy(alloc_, data_ + size_);
        }
    }
    
    void resize(size_type new_size) {
        if (new_size < size_) {
            for (size_type i = new_size; i < size_; ++i) {
                allocator_traits<Allocator>::destroy(alloc_, data_ + i);
            }
        } else if (new_size > size_) {
            reserve(new_size);
            for (size_type i = size_; i < new_size; ++i) {
                allocator_traits<Allocator>::construct(alloc_, data_ + i);
            }
        }
        size_ = new_size;
    }
    
    void resize(size_type new_size, const T& value) {
        if (new_size < size_) {
            for (size_type i = new_size; i < size_; ++i) {
                allocator_traits<Allocator>::destroy(alloc_, data_ + i);
            }
        } else if (new_size > size_) {
            reserve(new_size);
            for (size_type i = size_; i < new_size; ++i) {
                allocator_traits<Allocator>::construct(alloc_, data_ + i, value);
            }
        }
        size_ = new_size;
    }
    
    void swap(vector& other) noexcept {
        using std::swap;
        if constexpr (allocator_traits<Allocator>::propagate_on_container_swap::value) {
            swap(alloc_, other.alloc_);
        }
        swap(data_, other.data_);
        swap(size_, other.size_);
        swap(capacity_, other.capacity_);
    }

private:
    pointer data_;
    size_type size_;
    size_type capacity_;
    allocator_type alloc_;
    
    void reallocate(size_type new_capacity) {
        pointer new_data = alloc_.allocate(new_capacity);
        
        // 移动现有元素到新内存
        for (size_type i = 0; i < size_; ++i) {
            allocator_traits<Allocator>::construct(alloc_, new_data + i, std::move(data_[i]));
            allocator_traits<Allocator>::destroy(alloc_, data_ + i);
        }
        
        // 释放旧内存
        if (data_) {
            alloc_.deallocate(data_, capacity_);
        }
        
        data_ = new_data;
        capacity_ = new_capacity;
    }
};

// vector比较运算符
template<typename T, typename Allocator>
bool operator==(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

template<typename T, typename Allocator>
bool operator!=(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs) {
    return !(lhs == rhs);
}

template<typename T, typename Allocator>
bool operator<(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T, typename Allocator>
bool operator<=(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs) {
    return !(rhs < lhs);
}

template<typename T, typename Allocator>
bool operator>(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs) {
    return rhs < lhs;
}

template<typename T, typename Allocator>
bool operator>=(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs) {
    return !(lhs < rhs);
}

// vector特化的swap函数
template<typename T, typename Allocator>
void swap(vector<T, Allocator>& lhs, vector<T, Allocator>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace stl