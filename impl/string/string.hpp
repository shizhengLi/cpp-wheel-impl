#ifndef MY_STRING_HPP
#define MY_STRING_HPP

#include <iostream>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <iosfwd>

namespace my {

class string {
private:
    // 小字符串优化（SSO）
    static constexpr size_t SSO_MAX_SIZE = 15;
    static constexpr size_t SSO_BUFFER_SIZE = SSO_MAX_SIZE + 1;  // +1 for size marker
    union {
        struct {
            char* ptr;
            size_t size;
            size_t capacity;
            bool is_small;  // explicit flag
        } large;
        struct {
            char data[SSO_MAX_SIZE + 1];  // +1 for null terminator
            unsigned char size;  // size in low 7 bits, high bit is unused
        } small;
    } data_;
    
    bool is_small() const noexcept {
        return data_.large.is_small;
    }
    
    void set_small_size(size_t size) noexcept {
        data_.small.size = static_cast<unsigned char>(size);
    }
    
    size_t get_small_size() const noexcept {
        return data_.small.size;
    }
    
    void set_large_size(size_t size) noexcept {
        data_.large.size = size;
    }
    
    void set_large_capacity(size_t capacity) noexcept {
        data_.large.capacity = capacity;
    }
    
    void set_large_ptr(char* ptr) noexcept {
        data_.large.ptr = ptr;
    }
    
    char* get_ptr() noexcept {
        return is_small() ? data_.small.data : data_.large.ptr;
    }
    
    const char* get_ptr() const noexcept {
        return is_small() ? data_.small.data : data_.large.ptr;
    }
    
    void release_memory() {
        if (!is_small() && data_.large.ptr != nullptr) {
            delete[] data_.large.ptr;
            data_.large.ptr = nullptr;
        }
    }
    
    void init_from_cstr(const char* str) {
        size_t len = std::strlen(str);
        if (len <= SSO_MAX_SIZE) {
            // 小字符串优化
            std::memcpy(data_.small.data, str, len + 1);
            set_small_size(len);
            data_.large.is_small = true;  // 标记为小字符串
        } else {
            // 大字符串
            data_.large.ptr = new char[len + 1];
            std::memcpy(data_.large.ptr, str, len + 1);
            data_.large.size = len;
            data_.large.capacity = len;
            data_.large.is_small = false;  // 标记为大字符串
        }
    }
    
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
    
public:
    // 类型定义
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using value_type = char;
    using reference = char&;
    using const_reference = const char&;
    using pointer = char*;
    using const_pointer = const char*;
    
    class iterator {
    private:
        char* ptr_;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = char;
        using difference_type = ptrdiff_t;
        using pointer = char*;
        using reference = char&;
        
        iterator(pointer ptr) : ptr_(ptr) {}
        
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
    
    class const_iterator {
    private:
        const char* ptr_;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = char;
        using difference_type = ptrdiff_t;
        using pointer = const char*;
        using reference = const char&;
        
        const_iterator(const char* ptr) : ptr_(ptr) {}
        
        const_reference operator*() const { return *ptr_; }
        const_pointer operator->() const { return ptr_; }
        
        const_iterator& operator++() { ++ptr_; return *this; }
        const_iterator operator++(int) { const_iterator tmp = *this; ++ptr_; return tmp; }
        const_iterator& operator--() { --ptr_; return *this; }
        const_iterator operator--(int) { const_iterator tmp = *this; --ptr_; return tmp; }
        
        bool operator==(const const_iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const const_iterator& other) const { return ptr_ != other.ptr_; }
        
        difference_type operator-(const const_iterator& other) const { return ptr_ - other.ptr_; }
        const_iterator operator+(difference_type n) const { return const_iterator(ptr_ + n); }
        const_iterator operator-(difference_type n) const { return const_iterator(ptr_ - n); }
    };
    
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    static constexpr size_type npos = static_cast<size_type>(-1);
    
    // 构造函数
    string() noexcept {
        data_.small.data[0] = '\0';
        set_small_size(0);
        data_.large.is_small = true;  // 标记为小字符串
    }
    
    string(const char* str) {
        if (str == nullptr) {
            data_.small.data[0] = '\0';
            set_small_size(0);
            data_.large.is_small = true;  // 标记为小字符串
        } else {
            init_from_cstr(str);
        }
    }
    
    string(const char* str, size_type count) {
        if (count <= SSO_MAX_SIZE) {
            // 小字符串优化
            std::memcpy(data_.small.data, str, count);
            data_.small.data[count] = '\0';
            set_small_size(count);
            data_.large.is_small = true;  // 标记为小字符串
        } else {
            // 大字符串
            data_.large.ptr = new char[count + 1];
            std::memcpy(data_.large.ptr, str, count);
            data_.large.ptr[count] = '\0';
            data_.large.size = count;
            data_.large.capacity = count;
            data_.large.is_small = false;  // 标记为大字符串
        }
    }
    
    string(size_type count, char ch) {
        if (count <= SSO_MAX_SIZE) {
            // 小字符串优化
            std::memset(data_.small.data, ch, count);
            data_.small.data[count] = '\0';
            set_small_size(count);
            data_.large.is_small = true;  // 标记为小字符串
        } else {
            // 大字符串
            data_.large.ptr = new char[count + 1];
            std::memset(data_.large.ptr, ch, count);
            data_.large.ptr[count] = '\0';
            data_.large.size = count;
            data_.large.capacity = count;
            data_.large.is_small = false;  // 标记为大字符串
        }
    }
    
    string(const string& other) {
        size_t other_size = other.size();
        if (other_size <= SSO_MAX_SIZE) {
            // 小字符串优化
            std::memcpy(data_.small.data, other.data_.small.data, other_size + 1);
            set_small_size(other_size);
            data_.large.is_small = true;  // 标记为小字符串
        } else {
            // 大字符串
            data_.large.ptr = new char[other_size + 1];
            std::memcpy(data_.large.ptr, other.data_.large.ptr, other_size + 1);
            data_.large.size = other_size;
            data_.large.capacity = other_size;
            data_.large.is_small = false;  // 标记为大字符串
        }
    }
    
    string(string&& other) noexcept {
        if (other.is_small()) {
            // 小字符串直接拷贝
            std::memcpy(data_.small.data, other.data_.small.data, other.size() + 1);
            set_small_size(other.size());
            data_.large.is_small = true;  // 标记为小字符串
            other.data_.small.data[0] = '\0';
            other.set_small_size(0);
            other.data_.large.is_small = true;
        } else {
            // 大字符串转移所有权
            data_.large.ptr = other.data_.large.ptr;
            data_.large.size = other.data_.large.size;
            data_.large.capacity = other.data_.large.capacity;
            data_.large.is_small = false;  // 标记为大字符串
            
            other.data_.large.ptr = nullptr;
            other.data_.large.size = 0;
            other.data_.large.capacity = 0;
            other.data_.large.is_small = true;
        }
    }
    
    ~string() {
        release_memory();
    }
    
    // 赋值运算符
    string& operator=(const string& other) {
        if (this != &other) {
            string temp(other);
            swap(temp);
        }
        return *this;
    }
    
    string& operator=(string&& other) noexcept {
        if (this != &other) {
            release_memory();
            
            if (other.is_small()) {
                std::memcpy(data_.small.data, other.data_.small.data, other.size() + 1);
                set_small_size(other.size());
                data_.large.is_small = true;  // 标记为小字符串
                other.data_.small.data[0] = '\0';
                other.set_small_size(0);
                other.data_.large.is_small = true;
            } else {
                data_.large.ptr = other.data_.large.ptr;
                data_.large.size = other.data_.large.size;
                data_.large.capacity = other.data_.large.capacity;
                data_.large.is_small = false;  // 标记为大字符串
                
                other.data_.large.ptr = nullptr;
                other.data_.large.size = 0;
                other.data_.large.capacity = 0;
                other.data_.large.is_small = true;
            }
        }
        return *this;
    }
    
    string& operator=(const char* str) {
        string temp(str);
        swap(temp);
        return *this;
    }
    
    // 元素访问
    reference operator[](size_type pos) {
        return get_ptr()[pos];
    }
    
    const_reference operator[](size_type pos) const {
        return get_ptr()[pos];
    }
    
    reference at(size_type pos) {
        if (pos >= size()) {
            throw std::out_of_range("string::at");
        }
        return get_ptr()[pos];
    }
    
    const_reference at(size_type pos) const {
        if (pos >= size()) {
            throw std::out_of_range("string::at");
        }
        return get_ptr()[pos];
    }
    
    reference front() { return get_ptr()[0]; }
    const_reference front() const { return get_ptr()[0]; }
    
    reference back() { return get_ptr()[size() - 1]; }
    const_reference back() const { return get_ptr()[size() - 1]; }
    
    const char* data() const noexcept { return get_ptr(); }
    const char* c_str() const noexcept { return get_ptr(); }
    
    // 迭代器
    iterator begin() noexcept { return iterator(get_ptr()); }
    iterator end() noexcept { return iterator(get_ptr() + size()); }
    
    const_iterator begin() const noexcept { return const_iterator(get_ptr()); }
    const_iterator end() const noexcept { return const_iterator(get_ptr() + size()); }
    
    const_iterator cbegin() const noexcept { return const_iterator(get_ptr()); }
    const_iterator cend() const noexcept { return const_iterator(get_ptr() + size()); }
    
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }
    
    // 容量操作
    bool empty() const noexcept { return size() == 0; }
    
    size_type size() const noexcept {
        return is_small() ? get_small_size() : data_.large.size;
    }
    
    size_type length() const noexcept { return size(); }
    
    size_type capacity() const noexcept {
        return is_small() ? SSO_MAX_SIZE : data_.large.capacity;
    }
    
    void reserve(size_type new_cap) {
        if (new_cap > capacity()) {
            grow(new_cap);
        }
    }
    
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
    
    void clear() noexcept {
        if (!is_small()) {
            release_memory();
        }
        data_.small.data[0] = '\0';
        set_small_size(0);
        data_.large.is_small = true;  // 标记为小字符串
    }
    
    // 修改操作
    string& insert(size_type pos, const char* str) {
        size_type len = std::strlen(str);
        if (pos > size()) {
            throw std::out_of_range("string::insert");
        }
        
        size_type new_size = size() + len;
        reserve(new_size);
        
        char* ptr = get_ptr();
        std::memmove(ptr + pos + len, ptr + pos, size() - pos + 1);
        std::memcpy(ptr + pos, str, len);
        
        if (is_small()) {
            set_small_size(new_size);
        } else {
            data_.large.size = new_size;
        }
        
        return *this;
    }
    
    string& erase(size_type pos = 0, size_type count = npos) {
        if (pos > size()) {
            throw std::out_of_range("string::erase");
        }
        
        size_type actual_count = std::min(count, size() - pos);
        char* ptr = get_ptr();
        std::memmove(ptr + pos, ptr + pos + actual_count, size() - pos - actual_count + 1);
        
        size_type new_size = size() - actual_count;
        if (is_small()) {
            set_small_size(new_size);
        } else {
            data_.large.size = new_size;
        }
        
        return *this;
    }
    
    string& append(const char* str) {
        size_type len = std::strlen(str);
        size_type new_size = size() + len;
        reserve(new_size);
        
        std::memcpy(get_ptr() + size(), str, len + 1);
        
        if (is_small()) {
            set_small_size(new_size);
        } else {
            data_.large.size = new_size;
        }
        
        return *this;
    }
    
    string& append(size_type count, char ch) {
        size_type new_size = size() + count;
        reserve(new_size);
        
        std::memset(get_ptr() + size(), ch, count);
        get_ptr()[new_size] = '\0';
        
        if (is_small()) {
            set_small_size(new_size);
        } else {
            data_.large.size = new_size;
        }
        
        return *this;
    }
    
    string& append(const string& str) {
        return append(str.c_str());
    }
    
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
    
    string& operator+=(const char* str) {
        return append(str);
    }
    
    string& operator+=(const string& str) {
        return append(str);
    }
    
    string& operator+=(char ch) {
        return append(1, ch);
    }
    
    void resize(size_type new_size, char ch = '\0') {
        if (new_size < size()) {
            erase(new_size);
        } else if (new_size > size()) {
            append(new_size - size(), ch);
        }
    }
    
    void swap(string& other) noexcept {
        if (is_small() && other.is_small()) {
            // 两个都是小字符串，直接交换
            char temp[SSO_BUFFER_SIZE];
            std::memcpy(temp, data_.small.data, SSO_BUFFER_SIZE);
            std::memcpy(data_.small.data, other.data_.small.data, SSO_BUFFER_SIZE);
            std::memcpy(other.data_.small.data, temp, SSO_BUFFER_SIZE);
            
            // 交换size字段
            unsigned char temp_size = data_.small.size;
            data_.small.size = other.data_.small.size;
            other.data_.small.size = temp_size;
        } else {
            string temp(std::move(*this));
            *this = std::move(other);
            other = std::move(temp);
        }
    }
    
    // 字符串操作
    size_type find(const char* str, size_type pos = 0) const {
        size_type len = std::strlen(str);
        if (pos > size() || len > size() - pos) {
            return npos;
        }
        
        const char* result = std::strstr(get_ptr() + pos, str);
        return result ? result - get_ptr() : npos;
    }
    
    size_type find(char ch, size_type pos = 0) const {
        if (pos >= size()) {
            return npos;
        }
        
        const char* result = std::strchr(get_ptr() + pos, ch);
        return result ? result - get_ptr() : npos;
    }
    
    size_type rfind(const char* str, size_type pos = npos) const {
        size_type len = std::strlen(str);
        if (len > size()) {
            return npos;
        }
        
        pos = std::min(pos, size() - len);
        for (size_type i = pos + 1; i-- > 0;) {
            if (std::strncmp(get_ptr() + i, str, len) == 0) {
                return i;
            }
        }
        return npos;
    }
    
    size_type rfind(char ch, size_type pos = npos) const {
        pos = std::min(pos, size() - 1);
        for (size_type i = pos + 1; i-- > 0;) {
            if (get_ptr()[i] == ch) {
                return i;
            }
        }
        return npos;
    }
    
    string substr(size_type pos = 0, size_type count = npos) const {
        if (pos > size()) {
            throw std::out_of_range("string::substr");
        }
        
        size_type actual_count = std::min(count, size() - pos);
        return string(get_ptr() + pos, actual_count);
    }
    
    int compare(const string& other) const {
        return std::strcmp(get_ptr(), other.get_ptr());
    }
    
    // 比较运算符
    bool operator==(const string& other) const {
        return std::strcmp(get_ptr(), other.get_ptr()) == 0;
    }
    
    bool operator!=(const string& other) const {
        return std::strcmp(get_ptr(), other.get_ptr()) != 0;
    }
    
    bool operator<(const string& other) const {
        return std::strcmp(get_ptr(), other.get_ptr()) < 0;
    }
    
    bool operator<=(const string& other) const {
        return std::strcmp(get_ptr(), other.get_ptr()) <= 0;
    }
    
    bool operator>(const string& other) const {
        return std::strcmp(get_ptr(), other.get_ptr()) > 0;
    }
    
    bool operator>=(const string& other) const {
        return std::strcmp(get_ptr(), other.get_ptr()) >= 0;
    }
    
    bool operator==(const char* str) const {
        return std::strcmp(get_ptr(), str) == 0;
    }
    
    bool operator!=(const char* str) const {
        return std::strcmp(get_ptr(), str) != 0;
    }
    
    bool operator<(const char* str) const {
        return std::strcmp(get_ptr(), str) < 0;
    }
    
    bool operator<=(const char* str) const {
        return std::strcmp(get_ptr(), str) <= 0;
    }
    
    bool operator>(const char* str) const {
        return std::strcmp(get_ptr(), str) > 0;
    }
    
    bool operator>=(const char* str) const {
        return std::strcmp(get_ptr(), str) >= 0;
    }
};

// 非成员函数
string operator+(const string& lhs, const string& rhs) {
    string result(lhs);
    result += rhs;
    return result;
}

string operator+(const string& lhs, const char* rhs) {
    string result(lhs);
    result += rhs;
    return result;
}

string operator+(const char* lhs, const string& rhs) {
    string result(lhs);
    result += rhs;
    return result;
}

string operator+(const string& lhs, char rhs) {
    string result(lhs);
    result += rhs;
    return result;
}

string operator+(char lhs, const string& rhs) {
    string result(1, lhs);
    result += rhs;
    return result;
}

void swap(string& lhs, string& rhs) noexcept {
    lhs.swap(rhs);
}

// 输入输出运算符
std::ostream& operator<<(std::ostream& os, const string& str) {
    os << str.c_str();
    return os;
}

std::istream& operator>>(std::istream& is, string& str) {
    std::string temp;
    is >> temp;
    str = temp.c_str();
    return is;
}

std::istream& getline(std::istream& is, string& str, char delim = '\n') {
    std::string temp;
    std::getline(is, temp, delim);
    str = temp.c_str();
    return is;
}

} // namespace my

#endif // MY_STRING_HPP