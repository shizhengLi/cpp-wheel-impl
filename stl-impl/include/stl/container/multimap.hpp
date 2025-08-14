#ifndef STL_MULTIMAP_HPP
#define STL_MULTIMAP_HPP

#include "../tree/rb_tree.hpp"
#include "../functional.hpp"
#include <initializer_list>
#include <utility>

namespace stl {

// 用于从value_type（pair）中提取key的仿函数
template <typename Key, typename Value>
struct multimap_key_of_value {
    const Key& operator()(const std::pair<const Key, Value>& value) const {
        return value.first;
    }
};

// multimap容器实现
template <typename Key, typename Value, typename Compare = stl::less<Key>, typename Allocator = stl::allocator<std::pair<const Key, Value>>>
class multimap {
public:
    // 类型定义
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<const Key, Value>;
    using key_compare = Compare;
    using allocator_type = Allocator;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    
    // 值比较器
    class value_compare {
        friend class multimap;
    protected:
        Compare comp;
        value_compare(Compare c) : comp(c) {}
    public:
        bool operator()(const value_type& x, const value_type& y) const {
            return comp(x.first, y.first);
        }
    };
    
    // 内部红黑树类型
    using tree_type = rb_tree<key_type, value_type, multimap_key_of_value<key_type, mapped_type>, key_compare, allocator_type>;
    
    // 迭代器类型
    using iterator = typename tree_type::iterator;
    using const_iterator = typename tree_type::const_iterator;
    using reverse_iterator = stl::reverse_iterator<iterator>;
    using const_reverse_iterator = stl::reverse_iterator<const_iterator>;
    
    // 构造函数
    multimap() : tree_() {}
    
    explicit multimap(const key_compare& comp, const allocator_type& alloc = allocator_type())
        : tree_(comp) {}
    
    explicit multimap(const allocator_type& alloc) : tree_() {}
    
    template <typename InputIterator>
    multimap(InputIterator first, InputIterator last, 
        const key_compare& comp = key_compare(), 
        const allocator_type& alloc = allocator_type())
        : tree_(comp) {
        insert(first, last);
    }
    
    template <typename InputIterator>
    multimap(InputIterator first, InputIterator last, const allocator_type& alloc)
        : tree_() {
        insert(first, last);
    }
    
    multimap(std::initializer_list<value_type> init, 
        const key_compare& comp = key_compare(), 
        const allocator_type& alloc = allocator_type())
        : tree_(comp) {
        (void)alloc; // 避免未使用参数警告
        insert(init.begin(), init.end());
    }
    
    multimap(std::initializer_list<value_type> init, const allocator_type& alloc)
        : tree_() {
        insert(init.begin(), init.end());
    }
    
    multimap(const multimap& other) = default;
    
    multimap(multimap&& other) noexcept = default;
    
    ~multimap() = default;
    
    // 赋值运算符
    multimap& operator=(const multimap& other) = default;
    
    multimap& operator=(multimap&& other) noexcept = default;
    
    multimap& operator=(std::initializer_list<value_type> init) {
        clear();
        insert(init.begin(), init.end());
        return *this;
    }
    
    // 迭代器
    iterator begin() noexcept { return tree_.begin(); }
    const_iterator begin() const noexcept { return tree_.begin(); }
    const_iterator cbegin() const noexcept { return tree_.cbegin(); }
    
    iterator end() noexcept { return tree_.end(); }
    const_iterator end() const noexcept { return tree_.end(); }
    const_iterator cend() const noexcept { return tree_.cend(); }
    
    reverse_iterator rbegin() noexcept { return tree_.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return tree_.rbegin(); }
    const_reverse_iterator crbegin() const noexcept { return tree_.crbegin(); }
    
    reverse_iterator rend() noexcept { return tree_.rend(); }
    const_reverse_iterator rend() const noexcept { return tree_.rend(); }
    const_reverse_iterator crend() const noexcept { return tree_.crend(); }
    
    // 容量
    bool empty() const noexcept { return tree_.empty(); }
    size_type size() const noexcept { return tree_.size(); }
    size_type max_size() const noexcept { return tree_.max_size(); }
    
    // 修改器
    void clear() noexcept { tree_.clear(); }
    
    // 插入操作 - multimap允许重复键
    iterator insert(const value_type& value) {
        return tree_.insert_equal(value);
    }
    
    iterator insert(value_type&& value) {
        return tree_.insert_equal(std::move(value));
    }
    
    iterator insert(const_iterator hint, const value_type& value) {
        // 忽略hint，直接插入
        (void)hint; // 避免未使用参数警告
        return tree_.insert_equal(value);
    }
    
    iterator insert(const_iterator hint, value_type&& value) {
        // 忽略hint，直接插入
        (void)hint; // 避免未使用参数警告
        return tree_.insert_equal(std::move(value));
    }
    
    template <typename InputIterator>
    void insert(InputIterator first, InputIterator last) {
        for (; first != last; ++first) {
            tree_.insert_equal(*first);
        }
    }
    
    void insert(std::initializer_list<value_type> init) {
        insert(init.begin(), init.end());
    }
    
    template <typename... Args>
    iterator emplace(Args&&... args) {
        return tree_.insert_equal(value_type(std::forward<Args>(args)...));
    }
    
    template <typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args) {
        // 忽略hint，直接插入
        (void)hint; // 避免未使用参数警告
        return tree_.insert_equal(value_type(std::forward<Args>(args)...));
    }
    
    iterator erase(const_iterator pos) {
        // 简化的实现：找到键并删除
        iterator next = tree_.lower_bound(pos->first);
        ++next;
        tree_.erase(tree_.find(pos->first));
        return next;
    }
    
    iterator erase(iterator pos) {
        // 简化的实现：找到键并删除
        iterator next = pos;
        ++next;
        tree_.erase(pos);
        return next;
    }
    
    iterator erase(const_iterator first, const_iterator last) {
        // 简单实现：逐个删除
        while (first != last) {
            first = erase(first);
        }
        return begin();
    }
    
    size_type erase(const key_type& key) {
        size_type count = 0;
        iterator it = tree_.lower_bound(key);
        
        while (it != end() && it->first == key) {
            iterator next = it;
            ++next;
            tree_.erase(it);
            it = next;
            count++;
        }
        
        return count;
    }
    
    void swap(multimap& other) noexcept {
        tree_.swap(other.tree_);
    }
    
    // 查找
    size_type count(const key_type& key) const {
        size_type result = 0;
        auto range = equal_range(key);
        for (auto it = range.first; it != range.second; ++it) {
            result++;
        }
        return result;
    }
    
    iterator find(const key_type& key) {
        return tree_.find(key);
    }
    
    const_iterator find(const key_type& key) const {
        return tree_.find(key);
    }
    
    bool contains(const key_type& key) const {
        return find(key) != end();
    }
    
    std::pair<iterator, iterator> equal_range(const key_type& key) {
        return tree_.equal_range(key);
    }
    
    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return tree_.equal_range(key);
    }
    
    iterator lower_bound(const key_type& key) {
        return tree_.lower_bound(key);
    }
    
    const_iterator lower_bound(const key_type& key) const {
        return tree_.lower_bound(key);
    }
    
    iterator upper_bound(const key_type& key) {
        return tree_.upper_bound(key);
    }
    
    const_iterator upper_bound(const key_type& key) const {
        return tree_.upper_bound(key);
    }
    
    // 观察器
    key_compare key_comp() const { return tree_.key_comp(); }
    value_compare value_comp() const { return value_compare(tree_.key_comp()); }
    
    // 比较运算符
    bool operator==(const multimap& other) const {
        if (size() != other.size()) return false;
        
        auto it1 = begin();
        auto it2 = other.begin();
        
        while (it1 != end()) {
            if (it1->first != it2->first || it1->second != it2->second) {
                return false;
            }
            ++it1;
            ++it2;
        }
        
        return true;
    }
    
    bool operator!=(const multimap& other) const {
        return !(*this == other);
    }
    
    bool operator<(const multimap& other) const {
        return std::lexicographical_compare(begin(), end(), other.begin(), other.end(), value_comp());
    }
    
    bool operator<=(const multimap& other) const {
        return !(other < *this);
    }
    
    bool operator>(const multimap& other) const {
        return other < *this;
    }
    
    bool operator>=(const multimap& other) const {
        return !(*this < other);
    }
    
private:
    tree_type tree_;
};

// multimap交换特化
template <typename Key, typename Value, typename Compare, typename Allocator>
void swap(multimap<Key, Value, Compare, Allocator>& a, multimap<Key, Value, Compare, Allocator>& b) noexcept {
    a.swap(b);
}

} // namespace stl

#endif // STL_MULTIMAP_HPP