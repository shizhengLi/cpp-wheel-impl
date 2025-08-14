#ifndef STL_MULTISET_HPP
#define STL_MULTISET_HPP

#include "../tree/rb_tree.hpp"
#include "../functional.hpp"
#include <initializer_list>

namespace stl {

// 用于从value_type中提取key的仿函数
template <typename Key>
struct multiset_key_of_value {
    const Key& operator()(const Key& value) const {
        return value;
    }
};

// multiset容器实现
template <typename Key, typename Compare = stl::less<Key>, typename Allocator = stl::allocator<Key>>
class multiset {
public:
    // 类型定义
    using key_type = Key;
    using value_type = Key;
    using key_compare = Compare;
    using value_compare = Compare;
    using allocator_type = Allocator;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type&;
    
    // 内部红黑树类型
    using tree_type = rb_tree<key_type, value_type, multiset_key_of_value<key_type>, key_compare, allocator_type>;
    
    // 迭代器类型
    using iterator = typename tree_type::iterator;
    using const_iterator = typename tree_type::const_iterator;
    using reverse_iterator = stl::reverse_iterator<iterator>;
    using const_reverse_iterator = stl::reverse_iterator<const_iterator>;
    
    // 构造函数
    multiset() : tree_() {}
    
    explicit multiset(const key_compare& comp, const allocator_type& alloc = allocator_type())
        : tree_(comp) {}
    
    explicit multiset(const allocator_type& alloc) : tree_() {}
    
    template <typename InputIterator>
    multiset(InputIterator first, InputIterator last, 
        const key_compare& comp = key_compare(), 
        const allocator_type& alloc = allocator_type())
        : tree_(comp) {
        insert(first, last);
    }
    
    template <typename InputIterator>
    multiset(InputIterator first, InputIterator last, const allocator_type& alloc)
        : tree_() {
        (void)alloc; // 避免未使用参数警告
        insert(first, last);
    }
    
    multiset(std::initializer_list<value_type> init, 
        const key_compare& comp = key_compare(), 
        const allocator_type& alloc = allocator_type())
        : tree_(comp) {
        (void)alloc; // 避免未使用参数警告
        insert(init.begin(), init.end());
    }
    
    multiset(std::initializer_list<value_type> init, const allocator_type& alloc)
        : tree_() {
        (void)alloc; // 避免未使用参数警告
        insert(init.begin(), init.end());
    }
    
    multiset(const multiset& other) = default;
    
    multiset(multiset&& other) noexcept = default;
    
    ~multiset() = default;
    
    // 赋值运算符
    multiset& operator=(const multiset& other) = default;
    
    multiset& operator=(multiset&& other) noexcept = default;
    
    multiset& operator=(std::initializer_list<value_type> init) {
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
    
    iterator insert(const value_type& value) {
        return tree_.insert_equal(value);
    }
    
    iterator insert(value_type&& value) {
        return tree_.insert_equal(std::move(value));
    }
    
    iterator insert(const_iterator hint, const value_type& value) {
        // 忽略hint，直接插入
        return tree_.insert_equal(value);
    }
    
    iterator insert(const_iterator hint, value_type&& value) {
        // 忽略hint，直接插入
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
        return tree_.insert_equal(value_type(std::forward<Args>(args)...));
    }
    
    iterator erase(const_iterator pos) {
        // 简化的实现：找到键并删除
        key_type key = *pos;
        tree_.erase(tree_.find(key));
        return tree_.lower_bound(key);
    }
    
    iterator erase(iterator pos) {
        // 简化的实现：找到键并删除
        key_type key = *pos;
        tree_.erase(tree_.find(key));
        return tree_.lower_bound(key);
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
        auto range = equal_range(key);
        while (range.first != range.second) {
            range.first = erase(range.first);
            count++;
        }
        return count;
    }
    
    void swap(multiset& other) noexcept {
        tree_.swap(other.tree_);
    }
    
    // 查找
    size_type count(const key_type& key) const {
        auto range = equal_range(key);
        size_type count = 0;
        auto it = range.first;
        while (it != range.second) {
            ++count;
            ++it;
        }
        return count;
    }
    
    iterator find(const key_type& key) {
        return tree_.find(key);
    }
    
    const_iterator find(const key_type& key) const {
        return tree_.find(key);
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
    value_compare value_comp() const { return tree_.key_comp(); }
    
    // 比较运算符
    bool operator==(const multiset& other) const {
        if (size() != other.size()) return false;
        
        auto it1 = begin();
        auto it2 = other.begin();
        while (it1 != end() && it2 != other.end()) {
            if (*it1 != *it2) return false;
            ++it1;
            ++it2;
        }
        return true;
    }
    
    bool operator!=(const multiset& other) const {
        return !(*this == other);
    }
    
    bool operator<(const multiset& other) const {
        return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
    }
    
    bool operator<=(const multiset& other) const {
        return !(other < *this);
    }
    
    bool operator>(const multiset& other) const {
        return other < *this;
    }
    
    bool operator>=(const multiset& other) const {
        return !(*this < other);
    }
    
private:
    tree_type tree_;
};

// multiset交换特化
template <typename Key, typename Compare, typename Allocator>
void swap(multiset<Key, Compare, Allocator>& a, multiset<Key, Compare, Allocator>& b) noexcept {
    a.swap(b);
}

} // namespace stl

#endif // STL_MULTISET_HPP