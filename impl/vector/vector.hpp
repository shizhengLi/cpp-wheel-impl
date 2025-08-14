#ifndef MY_VECTOR_HPP
#define MY_VECTOR_HPP

#include <iostream>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <initializer_list>
#include <memory>

namespace my {

template <typename T>
class vector {
private:
    T* data_;
    size_t size_;
    size_t capacity_;
    
    void reallocate(size_t new_capacity) {
        if (new_capacity == capacity_) return;
        if (new_capacity < size_) new_capacity = size_;  // 确保容量不小于大小
        
        T* new_data = static_cast<T*>(::operator new(new_capacity * sizeof(T)));
        
        // 移动现有元素
        for (size_t i = 0; i < size_; ++i) {
            try {
                new (&new_data[i]) T(std::move(data_[i]));
            } catch (...) {
                // 构造失败，清理已构造的元素
                for (size_t j = 0; j < i; ++j) {
                    new_data[j].~T();
                }
                ::operator delete(new_data);
                throw;
            }
        }
        
        // 销毁旧元素
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        
        ::operator delete(data_);
        data_ = new_data;
        capacity_ = new_capacity;
    }
    
    void grow_if_needed() {
        if (size_ >= capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            reallocate(new_capacity);
        }
    }
    
    void destroy_elements() {
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
    }
    
public:
    // 类型定义
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    
    class iterator {
    private:
        pointer ptr_;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        
        iterator(pointer ptr) : ptr_(ptr) {}
        
        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }
        
        iterator& operator++() { ++ptr_; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++ptr_; return tmp; }
        iterator& operator--() { --ptr_; return *this; }
        iterator operator--(int) { iterator tmp = *this; --ptr_; return tmp; }
        
        iterator& operator+=(difference_type n) { ptr_ += n; return *this; }
        iterator operator+(difference_type n) const { return iterator(ptr_ + n); }
        iterator& operator-=(difference_type n) { ptr_ -= n; return *this; }
        iterator operator-(difference_type n) const { return iterator(ptr_ - n); }
        
        difference_type operator-(const iterator& other) const { return ptr_ - other.ptr_; }
        
        reference operator[](difference_type n) const { return ptr_[n]; }
        
        bool operator==(const iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const iterator& other) const { return ptr_ != other.ptr_; }
        bool operator<(const iterator& other) const { return ptr_ < other.ptr_; }
        bool operator<=(const iterator& other) const { return ptr_ <= other.ptr_; }
        bool operator>(const iterator& other) const { return ptr_ > other.ptr_; }
        bool operator>=(const iterator& other) const { return ptr_ >= other.ptr_; }
    };
    
    class const_iterator {
    private:
        const_pointer ptr_;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        
        const_iterator(const_pointer ptr) : ptr_(ptr) {}
        
        const_reference operator*() const { return *ptr_; }
        const_pointer operator->() const { return ptr_; }
        
        const_iterator& operator++() { ++ptr_; return *this; }
        const_iterator operator++(int) { const_iterator tmp = *this; ++ptr_; return tmp; }
        const_iterator& operator--() { --ptr_; return *this; }
        const_iterator operator--(int) { const_iterator tmp = *this; --ptr_; return tmp; }
        
        const_iterator& operator+=(difference_type n) { ptr_ += n; return *this; }
        const_iterator operator+(difference_type n) const { return const_iterator(ptr_ + n); }
        const_iterator& operator-=(difference_type n) { ptr_ -= n; return *this; }
        const_iterator operator-(difference_type n) const { return const_iterator(ptr_ - n); }
        
        difference_type operator-(const const_iterator& other) const { return ptr_ - other.ptr_; }
        
        const_reference operator[](difference_type n) const { return ptr_[n]; }
        
        bool operator==(const const_iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const const_iterator& other) const { return ptr_ != other.ptr_; }
        bool operator<(const const_iterator& other) const { return ptr_ < other.ptr_; }
        bool operator<=(const const_iterator& other) const { return ptr_ <= other.ptr_; }
        bool operator>(const const_iterator& other) const { return ptr_ > other.ptr_; }
        bool operator>=(const const_iterator& other) const { return ptr_ >= other.ptr_; }
    };
    
    class reverse_iterator {
    private:
        iterator it_;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        reverse_iterator(const iterator& it) : it_(it) {}
        
        reference operator*() const { 
            iterator temp = it_;
            return *(--temp);
        }
        
        pointer operator->() const { 
            iterator temp = it_;
            return &*(--temp);
        }
        
        reverse_iterator& operator++() { --it_; return *this; }
        reverse_iterator operator++(int) { reverse_iterator tmp = *this; --it_; return tmp; }
        reverse_iterator& operator--() { ++it_; return *this; }
        reverse_iterator operator--(int) { reverse_iterator tmp = *this; ++it_; return tmp; }
        
        bool operator==(const reverse_iterator& other) const { return it_ == other.it_; }
        bool operator!=(const reverse_iterator& other) const { return it_ != other.it_; }
    };
    
    class const_reverse_iterator {
    private:
        const_iterator it_;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;
        
        const_reverse_iterator(const const_iterator& it) : it_(it) {}
        
        reference operator*() const { 
            const_iterator temp = it_;
            return *(--temp);
        }
        
        pointer operator->() const { 
            const_iterator temp = it_;
            return &*(--temp);
        }
        
        const_reverse_iterator& operator++() { --it_; return *this; }
        const_reverse_iterator operator++(int) { const_reverse_iterator tmp = *this; --it_; return tmp; }
        const_reverse_iterator& operator--() { ++it_; return *this; }
        const_reverse_iterator operator--(int) { const_reverse_iterator tmp = *this; ++it_; return tmp; }
        
        bool operator==(const const_reverse_iterator& other) const { return it_ == other.it_; }
        bool operator!=(const const_reverse_iterator& other) const { return it_ != other.it_; }
    };
    
    // 构造函数
    vector() noexcept : data_(nullptr), size_(0), capacity_(0) {}
    
    explicit vector(size_type count, const T& value = T()) {
        data_ = static_cast<T*>(::operator new(count * sizeof(T)));
        size_ = count;
        capacity_ = count;
        
        for (size_type i = 0; i < count; ++i) {
            try {
                new (&data_[i]) T(value);
            } catch (...) {
                // 构造失败，清理已构造的元素
                for (size_type j = 0; j < i; ++j) {
                    data_[j].~T();
                }
                ::operator delete(data_);
                data_ = nullptr;
                size_ = 0;
                capacity_ = 0;
                throw;
            }
        }
    }
    
    vector(const vector& other) {
        data_ = static_cast<T*>(::operator new(other.size_ * sizeof(T)));
        size_ = other.size_;
        capacity_ = other.size_;
        
        for (size_type i = 0; i < size_; ++i) {
            try {
                new (&data_[i]) T(other.data_[i]);
            } catch (...) {
                // 构造失败，清理已构造的元素
                for (size_type j = 0; j < i; ++j) {
                    data_[j].~T();
                }
                ::operator delete(data_);
                data_ = nullptr;
                size_ = 0;
                capacity_ = 0;
                throw;
            }
        }
    }
    
    vector(vector&& other) noexcept : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    
    vector(std::initializer_list<T> init) {
        data_ = static_cast<T*>(::operator new(init.size() * sizeof(T)));
        size_ = init.size();
        capacity_ = init.size();
        
        size_type i = 0;
        for (const auto& value : init) {
            try {
                new (&data_[i]) T(value);
                ++i;
            } catch (...) {
                // 构造失败，清理已构造的元素
                for (size_type j = 0; j < i; ++j) {
                    data_[j].~T();
                }
                ::operator delete(data_);
                data_ = nullptr;
                size_ = 0;
                capacity_ = 0;
                throw;
            }
        }
    }
    
    ~vector() {
        clear();
        ::operator delete(data_);
    }
    
    // 赋值运算符
    vector& operator=(const vector& other) {
        if (this != &other) {
            vector temp(other);
            swap(temp);
        }
        return *this;
    }
    
    vector& operator=(vector&& other) noexcept {
        if (this != &other) {
            clear();
            ::operator delete(data_);
            
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }
    
    vector& operator=(std::initializer_list<T> init) {
        vector temp(init);
        swap(temp);
        return *this;
    }
    
    // 元素访问
    reference operator[](size_type pos) { return data_[pos]; }
    const_reference operator[](size_type pos) const { return data_[pos]; }
    
    reference at(size_type pos) {
        if (pos >= size_) {
            throw std::out_of_range("vector::at");
        }
        return data_[pos];
    }
    
    const_reference at(size_type pos) const {
        if (pos >= size_) {
            throw std::out_of_range("vector::at");
        }
        return data_[pos];
    }
    
    reference front() { return data_[0]; }
    const_reference front() const { return data_[0]; }
    
    reference back() { return data_[size_ - 1]; }
    const_reference back() const { return data_[size_ - 1]; }
    
    pointer data() noexcept { return data_; }
    const_pointer data() const noexcept { return data_; }
    
    // 迭代器
    iterator begin() noexcept { return iterator(data_); }
    iterator end() noexcept { return iterator(data_ + size_); }
    
    const_iterator begin() const noexcept { return const_iterator(data_); }
    const_iterator end() const noexcept { return const_iterator(data_ + size_); }
    
    const_iterator cbegin() const noexcept { return const_iterator(data_); }
    const_iterator cend() const noexcept { return const_iterator(data_ + size_); }
    
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }
    
    // 容量操作
    bool empty() const noexcept { return size_ == 0; }
    size_type size() const noexcept { return size_; }
    size_type capacity() const noexcept { return capacity_; }
    
    void reserve(size_type new_cap) {
        if (new_cap > capacity_) {
            reallocate(new_cap);
        }
    }
    
    void shrink_to_fit() {
        if (capacity_ > size_) {
            reallocate(size_);
        }
    }
    
    // 修改操作
    void clear() noexcept {
        destroy_elements();
        size_ = 0;
    }
    
    iterator insert(const_iterator pos, const T& value) {
        size_type index = pos - cbegin();
        if (index > size_) {
            throw std::out_of_range("vector::insert");
        }
        
        grow_if_needed();
        
        // 移动元素
        for (size_type i = size_; i > index; --i) {
            try {
                new (&data_[i]) T(std::move(data_[i - 1]));
            } catch (...) {
                // 移动失败，清理已移动的元素
                for (size_type j = size_; j > i; --j) {
                    data_[j].~T();
                }
                throw;
            }
            data_[i - 1].~T();
        }
        
        // 插入新元素
        new (&data_[index]) T(value);
        ++size_;
        
        return iterator(data_ + index);
    }
    
    iterator erase(const_iterator pos) {
        size_type index = pos - cbegin();
        if (index >= size_) {
            throw std::out_of_range("vector::erase");
        }
        
        // 销毁要删除的元素
        data_[index].~T();
        
        // 移动剩余元素
        for (size_type i = index; i < size_ - 1; ++i) {
            try {
                new (&data_[i]) T(std::move(data_[i + 1]));
            } catch (...) {
                // 移动失败，清理已移动的元素
                for (size_type j = index; j < i; ++j) {
                    data_[j].~T();
                }
                throw;
            }
            data_[i + 1].~T();
        }
        
        --size_;
        return iterator(data_ + index);
    }
    
    void push_back(const T& value) {
        grow_if_needed();
        new (&data_[size_]) T(value);
        ++size_;
    }
    
    void push_back(T&& value) {
        grow_if_needed();
        new (&data_[size_]) T(std::move(value));
        ++size_;
    }
    
    template <typename... Args>
    void emplace_back(Args&&... args) {
        grow_if_needed();
        new (&data_[size_]) T(std::forward<Args>(args)...);
        ++size_;
    }
    
    void pop_back() {
        if (size_ > 0) {
            --size_;
            data_[size_].~T();
        }
    }
    
    void resize(size_type new_size, const T& value = T()) {
        if (new_size < size_) {
            // 缩小
            for (size_type i = new_size; i < size_; ++i) {
                data_[i].~T();
            }
            size_ = new_size;
        } else if (new_size > size_) {
            // 扩大
            reserve(new_size);
            for (size_type i = size_; i < new_size; ++i) {
                try {
                    new (&data_[i]) T(value);
                } catch (...) {
                    // 构造失败，清理已构造的元素
                    for (size_type j = size_; j < i; ++j) {
                        data_[j].~T();
                    }
                    throw;
                }
            }
            size_ = new_size;
        }
    }
    
    void swap(vector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
};

// 非成员函数
template <typename T>
void swap(vector<T>& lhs, vector<T>& rhs) noexcept {
    lhs.swap(rhs);
}

// 比较运算符
template <typename T>
bool operator==(const vector<T>& lhs, const vector<T>& rhs) {
    if (lhs.size() != rhs.size()) return false;
    for (size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) return false;
    }
    return true;
}

template <typename T>
bool operator!=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(lhs == rhs);
}

template <typename T>
bool operator<(const vector<T>& lhs, const vector<T>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T>
bool operator<=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(rhs < lhs);
}

template <typename T>
bool operator>(const vector<T>& lhs, const vector<T>& rhs) {
    return rhs < lhs;
}

template <typename T>
bool operator>=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(lhs < rhs);
}

} // namespace my

#endif // MY_VECTOR_HPP