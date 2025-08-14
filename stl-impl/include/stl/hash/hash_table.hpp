#ifndef STL_HASH_TABLE_HPP
#define STL_HASH_TABLE_HPP

#include "../allocator.hpp"
#include "../iterator.hpp"
#include "../functional.hpp"
#include "../vector.hpp"
#include <utility>
#include <initializer_list>
#include <stdexcept>

namespace stl {

// 哈希表节点状态
enum class hash_node_state {
    empty,
    occupied,
    deleted
};

// 哈希表节点结构
template <typename T, typename Allocator>
struct hash_table_node {
    using node_allocator = typename Allocator::template rebind<hash_table_node>::other;
    using node_pointer = hash_table_node*;
    
    T data;
    hash_node_state state;
    
    hash_table_node() : state(hash_node_state::empty) {}
    
    template <typename... Args>
    hash_table_node(Args&&... args) 
        : data(std::forward<Args>(args)...), state(hash_node_state::occupied) {}
};

// 哈希表迭代器
template <typename T, typename Ref, typename Ptr, typename BucketArray>
class hash_table_iterator {
public:
    using iterator_category = forward_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using reference = Ref;
    using pointer = Ptr;
    using node_pointer = hash_table_node<T, allocator<T>>*;
    using bucket_array_pointer = BucketArray*;
    
    hash_table_iterator() : node_(nullptr), bucket_array_(nullptr), bucket_index_(0) {}
    
    hash_table_iterator(node_pointer node, bucket_array_pointer bucket_array, size_t bucket_index)
        : node_(node), bucket_array_(bucket_array), bucket_index_(bucket_index) {}
    
    reference operator*() const { return node_->data; }
    pointer operator->() const { return &node_->data; }
    
    hash_table_iterator& operator++() {
        find_next_occupied();
        return *this;
    }
    
    hash_table_iterator operator++(int) {
        hash_table_iterator temp = *this;
        ++(*this);
        return temp;
    }
    
    bool operator==(const hash_table_iterator& other) const {
        return node_ == other.node_;
    }
    
    bool operator!=(const hash_table_iterator& other) const {
        return node_ != other.node_;
    }
    
private:
    node_pointer node_;
    bucket_array_pointer bucket_array_;
    size_t bucket_index_;
    
    void find_next_occupied() {
        if (!bucket_array_) return;
        
        size_t num_buckets = bucket_array_->size();
        for (size_t i = bucket_index_ + 1; i < num_buckets; ++i) {
            if ((*bucket_array_)[i] && (*bucket_array_)[i]->state == hash_node_state::occupied) {
                node_ = (*bucket_array_)[i];
                bucket_index_ = i;
                return;
            }
        }
        
        node_ = nullptr;
        bucket_index_ = num_buckets;
    }
    
    template <typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
    friend class hash_table;
};

// 哈希表实现
template <typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
class hash_table {
public:
    using key_type = Key;
    using value_type = Value;
    using hasher = Hash;
    using key_equal = KeyEqual;
    using allocator_type = Allocator;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    
    using node_type = hash_table_node<Value, Allocator>;
    using node_pointer = node_type*;
    using node_allocator = typename Allocator::template rebind<node_type>::other;
    
    using bucket_array = stl::vector<node_pointer, typename Allocator::template rebind<node_pointer>::other>;
    using bucket_array_pointer = bucket_array*;
    
    using iterator = hash_table_iterator<Value, reference, pointer, bucket_array>;
    using const_iterator = hash_table_iterator<Value, const_reference, const_pointer, const bucket_array>;
    
    // 构造函数
    hash_table() : buckets_(), size_(0), max_load_factor_(1.0f), hash_(), key_equal_() {
        rehash(7); // 默认桶数
    }
    
    explicit hash_table(size_type bucket_count, 
                        const hasher& hash = hasher(),
                        const key_equal& equal = key_equal(),
                        const allocator_type& alloc = allocator_type())
        : buckets_(bucket_count, nullptr, alloc), size_(0), max_load_factor_(1.0f), 
          hash_(hash), key_equal_(equal), alloc_(alloc) {
        rehash(bucket_count);
    }
    
    hash_table(const hash_table& other)
        : buckets_(other.buckets_.size(), nullptr, other.alloc_), 
          size_(0), max_load_factor_(other.max_load_factor_),
          hash_(other.hash_), key_equal_(other.key_equal_), alloc_(other.alloc_) {
        rehash(other.buckets_.size());
        for (const auto& value : other) {
            insert(value);
        }
    }
    
    hash_table(hash_table&& other) noexcept
        : buckets_(std::move(other.buckets_)), size_(other.size_),
          max_load_factor_(other.max_load_factor_), hash_(std::move(other.hash_)),
          key_equal_(std::move(other.key_equal_)), alloc_(std::move(other.alloc_)) {
        other.size_ = 0;
        other.buckets_.clear();
    }
    
    ~hash_table() {
        clear();
    }
    
    // 赋值运算符
    hash_table& operator=(const hash_table& other) {
        if (this != &other) {
            clear();
            hash_ = other.hash_;
            key_equal_ = other.key_equal_;
            max_load_factor_ = other.max_load_factor_;
            rehash(other.buckets_.size());
            for (const auto& value : other) {
                insert(value);
            }
        }
        return *this;
    }
    
    hash_table& operator=(hash_table&& other) noexcept {
        if (this != &other) {
            clear();
            buckets_ = std::move(other.buckets_);
            size_ = other.size_;
            max_load_factor_ = other.max_load_factor_;
            hash_ = std::move(other.hash_);
            key_equal_ = std::move(other.key_equal_);
            alloc_ = std::move(other.alloc_);
            
            other.size_ = 0;
            other.buckets_.clear();
        }
        return *this;
    }
    
    // 迭代器
    iterator begin() {
        for (size_type i = 0; i < buckets_.size(); ++i) {
            if (buckets_[i] && buckets_[i]->state == hash_node_state::occupied) {
                return iterator(buckets_[i], &buckets_, i);
            }
        }
        return end();
    }
    
    const_iterator begin() const {
        for (size_type i = 0; i < buckets_.size(); ++i) {
            if (buckets_[i] && buckets_[i]->state == hash_node_state::occupied) {
                return const_iterator(buckets_[i], &buckets_, i);
            }
        }
        return end();
    }
    
    const_iterator cbegin() const { return begin(); }
    
    iterator end() { return iterator(nullptr, &buckets_, buckets_.size()); }
    const_iterator end() const { return const_iterator(nullptr, &buckets_, buckets_.size()); }
    const_iterator cend() const { return end(); }
    
    // 容量
    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }
    size_type max_size() const { return size_type(-1); }
    
    // 桶相关
    size_type bucket_count() const { return buckets_.size(); }
    size_type max_bucket_count() const { return size_type(-1); }
    
    float load_factor() const {
        return buckets_.empty() ? 0.0f : static_cast<float>(size_) / static_cast<float>(buckets_.size());
    }
    
    float max_load_factor() const { return max_load_factor_; }
    void max_load_factor(float ml) { max_load_factor_ = ml; }
    
    void rehash(size_type count) {
        if (count <= bucket_count()) return;
        
        bucket_array new_buckets(count, nullptr, alloc_);
        
        // 重新哈希所有元素
        for (size_type i = 0; i < buckets_.size(); ++i) {
            if (buckets_[i] && buckets_[i]->state == hash_node_state::occupied) {
                size_type new_index = hash_(get_key(buckets_[i]->data)) % count;
                
                // 处理冲突
                while (new_buckets[new_index] && new_buckets[new_index]->state == hash_node_state::occupied) {
                    new_index = (new_index + 1) % count;
                }
                
                new_buckets[new_index] = buckets_[i];
            }
        }
        
        buckets_ = std::move(new_buckets);
    }
    
    void reserve(size_type count) {
        rehash(static_cast<size_type>(count / max_load_factor()) + 1);
    }
    
    // 修改器
    void clear() {
        for (size_type i = 0; i < buckets_.size(); ++i) {
            if (buckets_[i]) {
                destroy_node(buckets_[i]);
                buckets_[i] = nullptr;
            }
        }
        size_ = 0;
    }
    
    std::pair<iterator, bool> insert(const value_type& value) {
        return insert_unique(value);
    }
    
    std::pair<iterator, bool> insert(value_type&& value) {
        return insert_unique(std::move(value));
    }
    
    iterator insert_multi(const value_type& value) {
        return insert_multi_impl(value);
    }
    
    iterator insert_multi(value_type&& value) {
        return insert_multi_impl(std::move(value));
    }
    
    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        return insert_unique(value_type(std::forward<Args>(args)...));
    }
    
    iterator erase(const_iterator pos) {
        return erase(iterator(pos.node_, const_cast<bucket_array_pointer>(pos.bucket_array_), pos.bucket_index_));
    }
    
    iterator erase(iterator pos) {
        if (pos == end()) return end();
        
        size_type index = pos.bucket_index_;
        if (index >= buckets_.size() || !buckets_[index] || buckets_[index]->state != hash_node_state::occupied) {
            return end();
        }
        
        buckets_[index]->state = hash_node_state::deleted;
        --size_;
        
        iterator next = pos;
        ++next;
        return next;
    }
    
    size_type erase(const key_type& key) {
        size_type count = 0;
        // Since we can't rely on equal_range for multi-element deletion,
        // we'll iterate through all elements and delete matching ones
        auto it = begin();
        while (it != end()) {
            if (key_equal_(get_key(*it), key)) {
                it = erase(it);
                count++;
            } else {
                ++it;
            }
        }
        return count;
    }
    
    // 查找
    iterator find(const key_type& key) {
        size_type index = find_bucket(key);
        if (index != buckets_.size() && buckets_[index] && buckets_[index]->state == hash_node_state::occupied) {
            return iterator(buckets_[index], &buckets_, index);
        }
        return end();
    }
    
    const_iterator find(const key_type& key) const {
        size_type index = find_bucket(key);
        if (index != buckets_.size() && buckets_[index] && buckets_[index]->state == hash_node_state::occupied) {
            return const_iterator(buckets_[index], &buckets_, index);
        }
        return end();
    }
    
    size_type count(const key_type& key) const {
        size_type count = 0;
        for (auto it = begin(); it != end(); ++it) {
            if (key_equal_(get_key(*it), key)) {
                count++;
            }
        }
        return count;
    }
    
    std::pair<iterator, iterator> equal_range(const key_type& key) {
        iterator first = end();
        iterator last = end();
        
        // Find first occurrence
        for (auto it = begin(); it != end(); ++it) {
            if (key_equal_(get_key(*it), key)) {
                first = it;
                break;
            }
        }
        
        if (first != end()) {
            // Find the position after the last occurrence
            // Since elements are not contiguous in hash tables with linear probing,
            // we need to find all matching elements and return end() as the second iterator
            // This is the standard behavior for unordered containers
            last = end();
        }
        
        return {first, last};
    }
    
    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        const_iterator first = end();
        const_iterator last = end();
        
        // Find first occurrence
        for (auto it = begin(); it != end(); ++it) {
            if (key_equal_(get_key(*it), key)) {
                first = it;
                break;
            }
        }
        
        if (first != end()) {
            // Find the position after the last occurrence
            // Since elements are not contiguous in hash tables with linear probing,
            // we need to find all matching elements and return end() as the second iterator
            // This is the standard behavior for unordered containers
            last = end();
        }
        
        return {first, last};
    }
    
    // 哈希策略
    hasher hash_function() const { return hash_; }
    key_equal key_eq() const { return key_equal_; }
    
    // 交换
    void swap(hash_table& other) noexcept {
        std::swap(buckets_, other.buckets_);
        std::swap(size_, other.size_);
        std::swap(max_load_factor_, other.max_load_factor_);
        std::swap(hash_, other.hash_);
        std::swap(key_equal_, other.key_equal_);
        std::swap(alloc_, other.alloc_);
    }
    
private:
    bucket_array buckets_;
    size_type size_;
    float max_load_factor_;
    hasher hash_;
    key_equal key_equal_;
    node_allocator alloc_;
    
    // 获取键的虚函数，需要在派生类中实现
    virtual const key_type& get_key(const value_type& value) const = 0;
    
    template <typename V>
    std::pair<iterator, bool> insert_unique(V&& value) {
        if (load_factor() >= max_load_factor_) {
            rehash(buckets_.size() * 2 + 1);
        }
        
        const key_type& key = get_key(value);
        size_type index = find_bucket(key);
        
        if (index != buckets_.size() && buckets_[index] && buckets_[index]->state == hash_node_state::occupied) {
            // 键已存在
            return {iterator(buckets_[index], &buckets_, index), false};
        }
        
        // 找到空位或删除位
        if (index == buckets_.size()) {
            index = hash_(key) % buckets_.size();
            while (buckets_[index] && buckets_[index]->state == hash_node_state::occupied) {
                index = (index + 1) % buckets_.size();
            }
        }
        
        if (!buckets_[index]) {
            buckets_[index] = create_node<V>(std::forward<V>(value));
        } else {
            // 重用删除的节点
            alloc_.destroy(&buckets_[index]->data);
            alloc_.construct(&buckets_[index]->data, std::forward<V>(value));
            buckets_[index]->state = hash_node_state::occupied;
        }
        
        ++size_;
        return {iterator(buckets_[index], &buckets_, index), true};
    }
    
    template <typename V>
    iterator insert_multi_impl(V&& value) {
        if (load_factor() >= max_load_factor_) {
            rehash(buckets_.size() * 2 + 1);
        }
        
        const key_type& key = get_key(value);
        size_type index = hash_(key) % buckets_.size();
        
        // 线性探测寻找插入位置，允许重复
        while (buckets_[index] && buckets_[index]->state == hash_node_state::occupied) {
            index = (index + 1) % buckets_.size();
        }
        
        if (!buckets_[index]) {
            buckets_[index] = create_node<V>(std::forward<V>(value));
        } else {
            // 重用删除的节点
            alloc_.destroy(&buckets_[index]->data);
            alloc_.construct(&buckets_[index]->data, std::forward<V>(value));
            buckets_[index]->state = hash_node_state::occupied;
        }
        
        ++size_;
        return iterator(buckets_[index], &buckets_, index);
    }
    
    size_type find_bucket(const key_type& key) const {
        if (buckets_.empty()) return buckets_.size();
        
        size_type index = hash_(key) % buckets_.size();
        size_type original_index = index;
        
        do {
            if (buckets_[index]) {
                if (buckets_[index]->state == hash_node_state::occupied) {
                    if (key_equal_(get_key(buckets_[index]->data), key)) {
                        return index;
                    }
                } else if (buckets_[index]->state == hash_node_state::empty) {
                    break;
                }
            } else {
                break;
            }
            
            index = (index + 1) % buckets_.size();
        } while (index != original_index);
        
        return buckets_.size();
    }
    
    template <typename... Args>
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

} // namespace stl

#endif // STL_HASH_TABLE_HPP