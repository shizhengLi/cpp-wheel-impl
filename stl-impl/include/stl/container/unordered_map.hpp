#ifndef STL_UNORDERED_MAP_HPP
#define STL_UNORDERED_MAP_HPP

#include "../hash/hash_table.hpp"
#include "../functional.hpp"
#include <initializer_list>
#include <utility>

namespace stl {

// 用于从value_type中提取key的仿函数
template <typename Key, typename Value>
struct unordered_map_key_of_value {
    const Key& operator()(const std::pair<const Key, Value>& value) const {
        return value.first;
    }
};

// unordered_map容器实现
template <typename Key, typename Value, typename Hash = stl::hash<Key>, typename KeyEqual = stl::equal_to<Key>, typename Allocator = stl::allocator<std::pair<const Key, Value>>>
class unordered_map {
public:
    // 类型定义
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<const Key, Value>;
    using hasher = Hash;
    using key_equal = KeyEqual;
    using allocator_type = Allocator;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
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
    unordered_map() : table_() {}
    
    explicit unordered_map(size_type bucket_count,
                          const hasher& hash = hasher(),
                          const key_equal& equal = key_equal(),
                          const allocator_type& alloc = allocator_type())
        : table_(bucket_count, hash, equal, alloc) {}
    
    explicit unordered_map(const allocator_type& alloc) : table_(0, hasher(), key_equal(), alloc) {}
    
    template <typename InputIterator>
    unordered_map(InputIterator first, InputIterator last,
                  size_type bucket_count = 0,
                  const hasher& hash = hasher(),
                  const key_equal& equal = key_equal(),
                  const allocator_type& alloc = allocator_type())
        : table_(bucket_count, hash, equal, alloc) {
        insert(first, last);
    }
    
    template <typename InputIterator>
    unordered_map(InputIterator first, InputIterator last, size_type bucket_count, const allocator_type& alloc)
        : table_(bucket_count, hasher(), key_equal(), alloc) {
        insert(first, last);
    }
    
    template <typename InputIterator>
    unordered_map(InputIterator first, InputIterator last, size_type bucket_count, const hasher& hash, const allocator_type& alloc)
        : table_(bucket_count, hash, key_equal(), alloc) {
        insert(first, last);
    }
    
    unordered_map(std::initializer_list<value_type> init,
                  size_type bucket_count = 0,
                  const hasher& hash = hasher(),
                  const key_equal& equal = key_equal(),
                  const allocator_type& alloc = allocator_type())
        : table_(bucket_count, hash, equal, alloc) {
        insert(init.begin(), init.end());
    }
    
    unordered_map(std::initializer_list<value_type> init, size_type bucket_count, const allocator_type& alloc)
        : table_(bucket_count, hasher(), key_equal(), alloc) {
        insert(init.begin(), init.end());
    }
    
    unordered_map(std::initializer_list<value_type> init, size_type bucket_count, const hasher& hash, const allocator_type& alloc)
        : table_(bucket_count, hash, key_equal(), alloc) {
        insert(init.begin(), init.end());
    }
    
    unordered_map(const unordered_map& other) = default;
    
    unordered_map(unordered_map&& other) noexcept = default;
    
    ~unordered_map() = default;
    
    // 赋值运算符
    unordered_map& operator=(const unordered_map& other) = default;
    
    unordered_map& operator=(unordered_map&& other) noexcept = default;
    
    unordered_map& operator=(std::initializer_list<value_type> init) {
        clear();
        insert(init.begin(), init.end());
        return *this;
    }
    
    // 元素访问
    mapped_type& operator[](const key_type& key) {
        auto result = table_.insert(value_type(key, mapped_type()));
        return result.first->second;
    }
    
    mapped_type& operator[](key_type&& key) {
        auto result = table_.insert(value_type(std::move(key), mapped_type()));
        return result.first->second;
    }
    
    mapped_type& at(const key_type& key) {
        auto it = table_.find(key);
        if (it == table_.end()) {
            throw std::out_of_range("unordered_map::at: key not found");
        }
        return it->second;
    }
    
    const mapped_type& at(const key_type& key) const {
        auto it = table_.find(key);
        if (it == table_.end()) {
            throw std::out_of_range("unordered_map::at: key not found");
        }
        return it->second;
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
    
    std::pair<iterator, bool> insert(const value_type& value) {
        return table_.insert(value);
    }
    
    std::pair<iterator, bool> insert(value_type&& value) {
        return table_.insert(std::move(value));
    }
    
    template <typename P>
    std::pair<iterator, bool> insert(P&& value) {
        return table_.insert(value_type(std::forward<P>(value)));
    }
    
    iterator insert(const_iterator hint, const value_type& value) {
        // 忽略hint，直接插入
        return table_.insert(value).first;
    }
    
    iterator insert(const_iterator hint, value_type&& value) {
        // 忽略hint，直接插入
        return table_.insert(std::move(value)).first;
    }
    
    template <typename P>
    iterator insert(const_iterator hint, P&& value) {
        // 忽略hint，直接插入
        return table_.insert(value_type(std::forward<P>(value))).first;
    }
    
    template <typename InputIterator>
    void insert(InputIterator first, InputIterator last) {
        for (; first != last; ++first) {
            table_.insert(*first);
        }
    }
    
    void insert(std::initializer_list<value_type> init) {
        insert(init.begin(), init.end());
    }
    
    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        return table_.emplace(std::forward<Args>(args)...);
    }
    
    template <typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args) {
        // 忽略hint，直接插入
        return table_.emplace(std::forward<Args>(args)...).first;
    }
    
    template <typename... Args>
    std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args) {
        auto it = table_.find(key);
        if (it != table_.end()) {
            return {it, false};
        }
        return table_.insert(value_type(key, mapped_type(std::forward<Args>(args)...)));
    }
    
    template <typename... Args>
    std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args) {
        auto it = table_.find(key);
        if (it != table_.end()) {
            return {it, false};
        }
        return table_.insert(value_type(std::move(key), mapped_type(std::forward<Args>(args)...)));
    }
    
    template <typename... Args>
    iterator try_emplace(const_iterator hint, const key_type& key, Args&&... args) {
        // 忽略hint，直接插入
        return try_emplace(key, std::forward<Args>(args)...).first;
    }
    
    template <typename... Args>
    iterator try_emplace(const_iterator hint, key_type&& key, Args&&... args) {
        // 忽略hint，直接插入
        return try_emplace(std::move(key), std::forward<Args>(args)...).first;
    }
    
    template <typename M>
    std::pair<iterator, bool> insert_or_assign(const key_type& key, M&& obj) {
        auto result = table_.insert(value_type(key, std::forward<M>(obj)));
        if (!result.second) {
            result.first->second = std::forward<M>(obj);
        }
        return result;
    }
    
    template <typename M>
    std::pair<iterator, bool> insert_or_assign(key_type&& key, M&& obj) {
        auto result = table_.insert(value_type(std::move(key), std::forward<M>(obj)));
        if (!result.second) {
            result.first->second = std::forward<M>(obj);
        }
        return result;
    }
    
    template <typename M>
    iterator insert_or_assign(const_iterator hint, const key_type& key, M&& obj) {
        // 忽略hint，直接插入
        return insert_or_assign(key, std::forward<M>(obj)).first;
    }
    
    template <typename M>
    iterator insert_or_assign(const_iterator hint, key_type&& key, M&& obj) {
        // 忽略hint，直接插入
        return insert_or_assign(std::move(key), std::forward<M>(obj)).first;
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
    
    size_type erase(const key_type& key) {
        return table_.erase(key);
    }
    
    void swap(unordered_map& other) noexcept {
        table_.swap(other.table_);
    }
    
    // 查找
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
    bool operator==(const unordered_map& other) const {
        if (size() != other.size()) return false;
        
        for (const auto& value : *this) {
            auto it = other.find(value.first);
            if (it == other.end() || it->second != value.second) {
                return false;
            }
        }
        
        return true;
    }
    
    bool operator!=(const unordered_map& other) const {
        return !(*this == other);
    }
    
private:
    // 继承哈希表并实现get_key方法
    class unordered_map_table : public table_type {
    public:
        using table_type::table_type;
        
    protected:
        const key_type& get_key(const value_type& value) const override {
            return value.first;
        }
    };
    
    unordered_map_table table_;
};

// unordered_map交换特化
template <typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
void swap(unordered_map<Key, Value, Hash, KeyEqual, Allocator>& a, 
         unordered_map<Key, Value, Hash, KeyEqual, Allocator>& b) noexcept {
    a.swap(b);
}

} // namespace stl

#endif // STL_UNORDERED_MAP_HPP