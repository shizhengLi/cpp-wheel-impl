#ifndef STL_UNORDERED_MULTIMAP_HPP
#define STL_UNORDERED_MULTIMAP_HPP

#include "../hash/hash_table.hpp"
#include "../functional.hpp"
#include <cstddef>
#include <initializer_list>
#include <utility>

namespace stl {

// 用于从value_type中提取key的仿函数
template <typename Key, typename Value>
struct unordered_multimap_key_of_value {
    const Key& operator()(const std::pair<const Key, Value>& value) const {
        return value.first;
    }
};

// unordered_multimap容器实现
template <typename Key, typename Value, typename Hash = stl::hash<Key>, typename KeyEqual = stl::equal_to<Key>, typename Allocator = stl::allocator<std::pair<const Key, Value>>>
class unordered_multimap {
public:
    // 类型定义
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<const Key, Value>;
    using hasher = Hash;
    using key_equal = KeyEqual;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    
    // 内部哈希表类型
    using table_type = hash_table<key_type, value_type, hasher, key_equal, allocator_type>;
    
    // 迭代器类型
    using iterator = typename table_type::iterator;
    using const_iterator = typename table_type::const_iterator;
    using local_iterator = iterator;
    using const_local_iterator = const_iterator;
    
    // 构造函数
    unordered_multimap() : table_() {}
    
    explicit unordered_multimap(size_type bucket_count,
                               const hasher& hash = hasher(),
                               const key_equal& equal = key_equal(),
                               const allocator_type& alloc = allocator_type())
        : table_(bucket_count, hash, equal, alloc) {}
    
    explicit unordered_multimap(const allocator_type& alloc) : table_(0, hasher(), key_equal(), alloc) {}
    
    template <typename InputIterator>
    unordered_multimap(InputIterator first, InputIterator last,
                      size_type bucket_count = 0,
                      const hasher& hash = hasher(),
                      const key_equal& equal = key_equal(),
                      const allocator_type& alloc = allocator_type())
        : table_(bucket_count, hash, equal, alloc) {
        insert(first, last);
    }
    
    template <typename InputIterator>
    unordered_multimap(InputIterator first, InputIterator last, size_type bucket_count, const allocator_type& alloc)
        : table_(bucket_count, hasher(), key_equal(), alloc) {
        insert(first, last);
    }
    
    template <typename InputIterator>
    unordered_multimap(InputIterator first, InputIterator last, size_type bucket_count, const hasher& hash, const allocator_type& alloc)
        : table_(bucket_count, hash, key_equal(), alloc) {
        insert(first, last);
    }
    
    unordered_multimap(std::initializer_list<value_type> init,
                      size_type bucket_count = 0,
                      const hasher& hash = hasher(),
                      const key_equal& equal = key_equal(),
                      const allocator_type& alloc = allocator_type())
        : table_(bucket_count, hash, equal, alloc) {
        insert(init.begin(), init.end());
    }
    
    unordered_multimap(std::initializer_list<value_type> init, size_type bucket_count, const allocator_type& alloc)
        : table_(bucket_count, hasher(), key_equal(), alloc) {
        insert(init.begin(), init.end());
    }
    
    unordered_multimap(std::initializer_list<value_type> init, size_type bucket_count, const hasher& hash, const allocator_type& alloc)
        : table_(bucket_count, hash, key_equal(), alloc) {
        insert(init.begin(), init.end());
    }
    
    unordered_multimap(const unordered_multimap& other) = default;
    
    unordered_multimap(unordered_multimap&& other) noexcept = default;
    
    ~unordered_multimap() = default;
    
    // 赋值运算符
    unordered_multimap& operator=(const unordered_multimap& other) = default;
    
    unordered_multimap& operator=(unordered_multimap&& other) noexcept = default;
    
    unordered_multimap& operator=(std::initializer_list<value_type> init) {
        clear();
        insert(init.begin(), init.end());
        return *this;
    }
    
    // 迭代器
    iterator begin() noexcept { return table_.begin(); }
    const_iterator begin() const noexcept { return table_.begin(); }
    const_iterator cbegin() const noexcept { return table_.cbegin(); }
    
    iterator end() noexcept { return table_.end(); }
    const_iterator end() const noexcept { return table_.end(); }
    const_iterator cend() const noexcept { return table_.cend(); }
    
    // 容量
    bool empty() const noexcept { return table_.empty(); }
    size_type size() const noexcept { return table_.size(); }
    size_type max_size() const noexcept { return table_.max_size(); }
    
    // 修改器
    void clear() noexcept { table_.clear(); }
    
    // 支持真正的重复键插入
    iterator insert(const value_type& value) {
        return table_.insert_multi(value);
    }
    
    iterator insert(value_type&& value) {
        return table_.insert_multi(std::move(value));
    }
    
    template <typename P>
    iterator insert(P&& value) {
        return table_.insert_multi(value_type(std::forward<P>(value)));
    }
    
    iterator insert(const_iterator hint, const value_type& value) {
        // 忽略hint，直接插入
        return table_.insert_multi(value);
    }
    
    iterator insert(const_iterator hint, value_type&& value) {
        // 忽略hint，直接插入
        return table_.insert_multi(std::move(value));
    }
    
    template <typename P>
    iterator insert(const_iterator hint, P&& value) {
        // 忽略hint，直接插入
        return table_.insert_multi(value_type(std::forward<P>(value)));
    }
    
    template <typename InputIterator>
    void insert(InputIterator first, InputIterator last) {
        for (; first != last; ++first) {
            table_.insert_multi(*first);
        }
    }
    
    void insert(std::initializer_list<value_type> init) {
        insert(init.begin(), init.end());
    }
    
    template <typename... Args>
    iterator emplace(Args&&... args) {
        return table_.insert_multi(value_type(std::forward<Args>(args)...));
    }
    
    template <typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args) {
        // 忽略hint，直接插入
        return table_.insert_multi(value_type(std::forward<Args>(args)...));
    }
    
    iterator erase(const_iterator pos) {
        return table_.erase(pos);
    }
    
    iterator erase(iterator pos) {
        return table_.erase(pos);
    }
    
    iterator erase(const_iterator first, const_iterator last) {
        // 简单实现：逐个删除
        while (first != last) {
            first = erase(first);
        }
        return begin();
    }
    
    // 支持删除所有重复键
    size_type erase(const key_type& key) {
        return table_.erase(key);
    }
    
    void swap(unordered_multimap& other) noexcept {
        table_.swap(other.table_);
    }
    
    // 查找
    // 支持返回重复键的个数
    size_type count(const key_type& key) const {
        return table_.count(key);
    }
    
    iterator find(const key_type& key) {
        return table_.find(key);
    }
    
    const_iterator find(const key_type& key) const {
        return table_.find(key);
    }
    
    std::pair<iterator, iterator> equal_range(const key_type& key) {
        return table_.equal_range(key);
    }
    
    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return table_.equal_range(key);
    }
    
    // 桶接口
    size_type bucket_count() const { return table_.bucket_count(); }
    size_type max_bucket_count() const { return table_.max_bucket_count(); }
    
    size_type bucket_size(size_type n) const {
        size_type count = 0;
        auto range = equal_range((*begin()).first); // 简化实现
        for (auto it = range.first; it != range.second; ++it) {
            ++count;
        }
        return count;
    }
    
    size_type bucket(const key_type& key) const {
        return table_.hash_function()(key) % bucket_count();
    }
    
    local_iterator begin(size_type n) {
        return begin(); // 简化实现
    }
    
    const_local_iterator begin(size_type n) const {
        return begin(); // 简化实现
    }
    
    local_iterator end(size_type n) {
        return end(); // 简化实现
    }
    
    const_local_iterator end(size_type n) const {
        return end(); // 简化实现
    }
    
    const_local_iterator cbegin(size_type n) const {
        return begin(); // 简化实现
    }
    
    const_local_iterator cend(size_type n) const {
        return end(); // 简化实现
    }
    
    // 哈希策略
    float load_factor() const { return table_.load_factor(); }
    float max_load_factor() const { return table_.max_load_factor(); }
    void max_load_factor(float ml) { table_.max_load_factor(ml); }
    
    void rehash(size_type count) { table_.rehash(count); }
    void reserve(size_type count) { table_.reserve(count); }
    
    // 观察器
    hasher hash_function() const { return table_.hash_function(); }
    key_equal key_eq() const { return table_.key_eq(); }
    
    // 比较运算符
    bool operator==(const unordered_multimap& other) const {
        if (size() != other.size()) return false;
        
        // 支持重复键的比较
        for (const auto& value : *this) {
            bool found = false;
            for (const auto& other_value : other) {
                if (key_eq()(value.first, other_value.first) && value.second == other_value.second) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
        
        return true;
    }
    
    bool operator!=(const unordered_multimap& other) const {
        return !(*this == other);
    }
    
private:
    // 继承哈希表并实现get_key方法
    class unordered_multimap_table : public table_type {
    public:
        using table_type::table_type;
        
    protected:
        const key_type& get_key(const value_type& value) const override {
            return value.first;
        }
    };
    
    unordered_multimap_table table_;
};

// unordered_multimap交换特化
template <typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
void swap(unordered_multimap<Key, Value, Hash, KeyEqual, Allocator>& a, 
         unordered_multimap<Key, Value, Hash, KeyEqual, Allocator>& b) noexcept {
    a.swap(b);
}

} // namespace stl

#endif // STL_UNORDERED_MULTIMAP_HPP