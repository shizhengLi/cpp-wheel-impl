#ifndef STL_RED_BLACK_TREE_HPP
#define STL_RED_BLACK_TREE_HPP

#include "../allocator.hpp"
#include "../iterator.hpp"
#include "../functional.hpp"
#include <utility>
#include <initializer_list>
#include <stdexcept>

namespace stl {

// 红黑树节点颜色
enum class rb_color {
    red,
    black
};

// 红黑树节点结构
template <typename T, typename Allocator>
struct rb_tree_node {
    using node_allocator = typename Allocator::template rebind<rb_tree_node>::other;
    using node_pointer = rb_tree_node*;
    
    T data;
    node_pointer left;
    node_pointer right;
    node_pointer parent;
    rb_color color;
    
    rb_tree_node(const T& value, node_pointer p = nullptr) 
        : data(value), left(nullptr), right(nullptr), parent(p), color(rb_color::red) {}
    
    rb_tree_node(T&& value, node_pointer p = nullptr) 
        : data(std::move(value)), left(nullptr), right(nullptr), parent(p), color(rb_color::red) {}
    
    template <typename... Args>
    rb_tree_node(Args&&... args, node_pointer p = nullptr) 
        : data(std::forward<Args>(args)...), left(nullptr), right(nullptr), parent(p), color(rb_color::red) {}
};

// 红黑树迭代器
template <typename T, typename Ref, typename Ptr>
class rb_tree_iterator {
public:
    using iterator_category = bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using reference = Ref;
    using pointer = Ptr;
    using node_pointer = rb_tree_node<T, allocator<T>>*;
    
    rb_tree_iterator() : node_(nullptr) {}
    explicit rb_tree_iterator(node_pointer node) : node_(node) {}
    
    reference operator*() const { return node_->data; }
    pointer operator->() const { return &node_->data; }
    
    rb_tree_iterator& operator++() {
        if (node_->right) {
            node_ = node_->right;
            while (node_->left) {
                node_ = node_->left;
            }
        } else {
            node_pointer parent = node_->parent;
            while (parent && node_ == parent->right) {
                node_ = parent;
                parent = parent->parent;
            }
            node_ = parent;
        }
        return *this;
    }
    
    rb_tree_iterator operator++(int) {
        rb_tree_iterator temp = *this;
        ++(*this);
        return temp;
    }
    
    rb_tree_iterator& operator--() {
        if (!node_) {
            // We're at end(), this is undefined behavior for decrement
            throw std::runtime_error("Cannot decrement end() iterator");
        }
        if (node_->left) {
            node_ = node_->left;
            while (node_->right) {
                node_ = node_->right;
            }
        } else {
            node_pointer parent = node_->parent;
            while (parent && node_ == parent->left) {
                node_ = parent;
                parent = parent->parent;
            }
            node_ = parent;
        }
        return *this;
    }
    
    rb_tree_iterator operator--(int) {
        if (!node_) {
            throw std::runtime_error("Cannot decrement end() iterator");
        }
        rb_tree_iterator temp = *this;
        --(*this);
        return temp;
    }
    
    bool operator==(const rb_tree_iterator& other) const {
        return node_ == other.node_;
    }
    
    bool operator!=(const rb_tree_iterator& other) const {
        return node_ != other.node_;
    }
    
    node_pointer get_node() const { return node_; }
    
private:
    node_pointer node_;
    
    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
    friend class rb_tree;
};

// 红黑树实现
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
class rb_tree {
public:
    using key_type = Key;
    using value_type = Value;
    using key_of_value = KeyOfValue;
    using key_compare = Compare;
    using allocator_type = Allocator;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    
    using node_type = rb_tree_node<Value, Allocator>;
    using node_pointer = node_type*;
    using node_allocator = typename Allocator::template rebind<node_type>::other;
    
    using iterator = rb_tree_iterator<Value, reference, pointer>;
    using const_iterator = rb_tree_iterator<Value, const_reference, const_pointer>;
    using reverse_iterator = stl::reverse_iterator<iterator>;
    using const_reverse_iterator = stl::reverse_iterator<const_iterator>;
    
    // 构造函数
    rb_tree() : root_(nullptr), size_(0) {}
    
    explicit rb_tree(const key_compare& comp) : root_(nullptr), size_(0), comp_(comp) {}
    
    rb_tree(const rb_tree& other) : root_(nullptr), size_(0), comp_(other.comp_) {
        copy_from(other);
    }
    
    rb_tree(rb_tree&& other) noexcept : root_(other.root_), size_(other.size_), comp_(std::move(other.comp_)) {
        other.root_ = nullptr;
        other.size_ = 0;
    }
    
    ~rb_tree() {
        clear();
    }
    
    // 赋值运算符
    rb_tree& operator=(const rb_tree& other) {
        if (this != &other) {
            clear();
            comp_ = other.comp_;
            copy_from(other);
        }
        return *this;
    }
    
    rb_tree& operator=(rb_tree&& other) noexcept {
        if (this != &other) {
            clear();
            root_ = other.root_;
            size_ = other.size_;
            comp_ = std::move(other.comp_);
            other.root_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    // 迭代器
    iterator begin() { return iterator(leftmost()); }
    const_iterator begin() const { return const_iterator(leftmost()); }
    const_iterator cbegin() const { return const_iterator(leftmost()); }
    
    iterator end() { return iterator(nullptr); }
    const_iterator end() const { return const_iterator(nullptr); }
    const_iterator cend() const { return const_iterator(nullptr); }
    
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }
    
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }
    
    // 容量
    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }
    size_type max_size() const { return size_type(-1); }
    
    // 修改器
    void clear() {
        destroy_tree(root_);
        root_ = nullptr;
        size_ = 0;
    }
    
    // 插入操作
    std::pair<iterator, bool> insert(const value_type& value) {
        return insert_unique(value);
    }
    
    std::pair<iterator, bool> insert(value_type&& value) {
        return insert_unique(std::move(value));
    }
    
    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        return insert_unique(value_type(std::forward<Args>(args)...));
    }
    
    // 允许重复的插入操作（用于multiset）
    iterator insert_equal(const value_type& value) {
        return insert_equal_impl(value);
    }
    
    iterator insert_equal(value_type&& value) {
        return insert_equal_impl(std::move(value));
    }
    
    // 删除操作
    iterator erase(iterator pos) {
        if (pos == end()) return pos;
        
        node_pointer node = pos.node_;
        iterator next = pos;
        ++next;
        
        // 处理有两个孩子的节点
        if (node->left && node->right) {
            // 找到后继节点（右子树的最左节点）
            node_pointer successor = node->right;
            while (successor->left) {
                successor = successor->left;
            }
            
            // 对于const key的限制，我们采用不同的策略
            // 我们不能直接赋值std::pair<const Key, Value>
            // 所以我们需要重新设计节点删除策略
            
            // 交换节点数据而不是赋值
            // 这样可以避免const key的问题
            // 我们使用节点数据的交换，而不是直接赋值
            
            // 创建临时存储
            value_type temp_data = std::move(node->data);
            
            // 移动后继节点的数据到当前节点
            // 由于key是const的，我们需要特殊的处理
            // 这里我们使用placement new和destroy来重建对象
            
            // 销毁当前节点的数据
            alloc_.destroy(&node->data);
            
            // 在当前节点的内存中构造后继节点的数据
            alloc_.construct(&node->data, std::move(successor->data));
            
            // 销毁后继节点的数据
            alloc_.destroy(&successor->data);
            
            // 在后继节点的内存中构造临时数据
            alloc_.construct(&successor->data, std::move(temp_data));
            
            // 设置要删除的节点为后继节点
            node = successor;
        }
        
        // 现在节点最多有一个孩子
        node_pointer child = node->left ? node->left : node->right;
        node_pointer parent = node->parent;
        
        if (child) {
            child->parent = parent;
        }
        
        if (!parent) {
            root_ = child;
        } else if (parent->left == node) {
            parent->left = child;
        } else {
            parent->right = child;
        }
        
        if (node->color == rb_color::black) {
            if (child) {
                erase_fixup(child);
            }
        }
        
        destroy_node(node);
        --size_;
        
        return next;
    }
    
    // 查找操作
    iterator find(const key_type& key) {
        node_pointer node = find_node(key);
        return node ? iterator(node) : end();
    }
    
    const_iterator find(const key_type& key) const {
        node_pointer node = find_node(key);
        return node ? const_iterator(node) : end();
    }
    
    size_type count(const key_type& key) const {
        return find_node(key) ? 1 : 0;
    }
    
    iterator lower_bound(const key_type& key) {
        node_pointer node = lower_bound_node(key);
        return node ? iterator(node) : end();
    }
    
    const_iterator lower_bound(const key_type& key) const {
        node_pointer node = lower_bound_node(key);
        return node ? const_iterator(node) : end();
    }
    
    iterator upper_bound(const key_type& key) {
        node_pointer node = upper_bound_node(key);
        return node ? iterator(node) : end();
    }
    
    const_iterator upper_bound(const key_type& key) const {
        node_pointer node = upper_bound_node(key);
        return node ? const_iterator(node) : end();
    }
    
    std::pair<iterator, iterator> equal_range(const key_type& key) {
        return {lower_bound(key), upper_bound(key)};
    }
    
    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return {lower_bound(key), upper_bound(key)};
    }
    
    // 比较器访问
    key_compare key_comp() const { return comp_; }
    
    // 红黑树验证
    bool is_valid_rb_tree() const;
    void verify_properties() const;
    
    // 交换操作
    void swap(rb_tree& other) noexcept {
        std::swap(root_, other.root_);
        std::swap(size_, other.size_);
        std::swap(comp_, other.comp_);
        std::swap(alloc_, other.alloc_);
    }
    
private:
    node_pointer root_;
    size_type size_;
    key_compare comp_;
    node_allocator alloc_;
    
    // 红黑树删除修复
    void erase_fixup(node_pointer node);
    
    // 允许重复的插入实现
    template <typename V>
    iterator insert_equal_impl(V&& value) {
        node_pointer parent = nullptr;
        node_pointer* link = &root_;
        
        while (*link) {
            parent = *link;
            if (comp_(key_of_value()(value), key_of_value()(parent->data))) {
                link = &parent->left;
            } else {
                // 对于multiset，相等的元素插入到右子树
                link = &parent->right;
            }
        }
        
        node_pointer new_node = create_node<V>(std::forward<V>(value), parent);
        *link = new_node;
        ++size_;
        
        // 红黑树插入修复
        insert_fixup(new_node);
        
        return iterator(new_node);
    }
    
    // 辅助函数
    node_pointer leftmost() const {
        if (!root_) return nullptr;
        node_pointer node = root_;
        while (node->left) {
            node = node->left;
        }
        return node;
    }
    
    node_pointer rightmost() const {
        if (!root_) return nullptr;
        node_pointer node = root_;
        while (node->right) {
            node = node->right;
        }
        return node;
    }
    
    node_pointer find_node(const key_type& key) const {
        node_pointer node = root_;
        while (node) {
            if (comp_(key, key_of_value()(node->data))) {
                node = node->left;
            } else if (comp_(key_of_value()(node->data), key)) {
                node = node->right;
            } else {
                return node;
            }
        }
        return nullptr;
    }
    
    node_pointer lower_bound_node(const key_type& key) const {
        node_pointer node = root_;
        node_pointer result = nullptr;
        while (node) {
            if (comp_(key_of_value()(node->data), key)) {
                // node->data < key, need to go right
                node = node->right;
            } else {
                // node->data >= key, this is a candidate
                result = node;
                node = node->left;
            }
        }
        return result;
    }
    
    node_pointer upper_bound_node(const key_type& key) const {
        node_pointer node = root_;
        node_pointer result = nullptr;
        while (node) {
            if (comp_(key, key_of_value()(node->data))) {
                result = node;
                node = node->left;
            } else {
                node = node->right;
            }
        }
        return result;
    }
    
    template <typename V>
    std::pair<iterator, bool> insert_unique(V&& value) {
        node_pointer parent = nullptr;
        node_pointer* link = &root_;
        
        while (*link) {
            parent = *link;
            if (comp_(key_of_value()(value), key_of_value()(parent->data))) {
                link = &parent->left;
            } else if (comp_(key_of_value()(parent->data), key_of_value()(value))) {
                link = &parent->right;
            } else {
                // 键已存在
                return {iterator(parent), false};
            }
        }
        
        node_pointer new_node = create_node<V>(std::forward<V>(value), parent);
        *link = new_node;
        ++size_;
        
        // 红黑树插入修复
        insert_fixup(new_node);
        
        return {iterator(new_node), true};
    }
    
    void copy_from(const rb_tree& other) {
        root_ = copy_tree(other.root_, nullptr);
        size_ = other.size_;
    }
    
    node_pointer copy_tree(node_pointer node, node_pointer parent) {
        if (!node) return nullptr;
        
        node_pointer new_node = create_node<value_type const&>(node->data, parent);
        new_node->color = node->color;
        new_node->left = copy_tree(node->left, new_node);
        new_node->right = copy_tree(node->right, new_node);
        
        return new_node;
    }
    
    void destroy_tree(node_pointer node) {
        if (!node) return;
        
        destroy_tree(node->left);
        destroy_tree(node->right);
        destroy_node(node);
    }
    
    template <typename... Args>
    node_pointer create_node(Args&&... args, node_pointer parent) {
        node_pointer node = alloc_.allocate(1);
        try {
            alloc_.construct(node, std::forward<Args>(args)..., parent);
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
    
    // 红黑树操作
    void insert_fixup(node_pointer node);
    void left_rotate(node_pointer x);
    void right_rotate(node_pointer y);
    void transplant(node_pointer u, node_pointer v);
};

// 红黑树操作实现
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
void rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::insert_fixup(node_pointer node) {
    while (node != root_ && node->parent->color == rb_color::red) {
        if (node->parent == node->parent->parent->left) {
            node_pointer uncle = node->parent->parent->right;
            if (uncle && uncle->color == rb_color::red) {
                // 情况1：叔叔是红色
                node->parent->color = rb_color::black;
                uncle->color = rb_color::black;
                node->parent->parent->color = rb_color::red;
                node = node->parent->parent;
            } else {
                // 情况2：叔叔是黑色
                if (node == node->parent->right) {
                    node = node->parent;
                    left_rotate(node);
                }
                // 情况3：叔叔是黑色，节点是左孩子
                node->parent->color = rb_color::black;
                node->parent->parent->color = rb_color::red;
                right_rotate(node->parent->parent);
            }
        } else {
            // 对称情况
            node_pointer uncle = node->parent->parent->left;
            if (uncle && uncle->color == rb_color::red) {
                // 情况1：叔叔是红色
                node->parent->color = rb_color::black;
                uncle->color = rb_color::black;
                node->parent->parent->color = rb_color::red;
                node = node->parent->parent;
            } else {
                // 情况2：叔叔是黑色
                if (node == node->parent->left) {
                    node = node->parent;
                    right_rotate(node);
                }
                // 情况3：叔叔是黑色，节点是右孩子
                node->parent->color = rb_color::black;
                node->parent->parent->color = rb_color::red;
                left_rotate(node->parent->parent);
            }
        }
    }
    root_->color = rb_color::black;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
void rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::left_rotate(node_pointer x) {
    node_pointer y = x->right;
    x->right = y->left;
    
    if (y->left) {
        y->left->parent = x;
    }
    
    y->parent = x->parent;
    
    if (!x->parent) {
        root_ = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    
    y->left = x;
    x->parent = y;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
void rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::right_rotate(node_pointer y) {
    node_pointer x = y->left;
    y->left = x->right;
    
    if (x->right) {
        x->right->parent = y;
    }
    
    x->parent = y->parent;
    
    if (!y->parent) {
        root_ = x;
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }
    
    x->right = y;
    y->parent = x;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
void rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::transplant(node_pointer u, node_pointer v) {
    if (!u->parent) {
        root_ = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    
    if (v) {
        v->parent = u->parent;
    }
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
void rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::erase_fixup(node_pointer node) {
    while (node != root_ && (!node || node->color == rb_color::black)) {
        if (node == node->parent->left) {
            node_pointer sibling = node->parent->right;
            if (sibling && sibling->color == rb_color::red) {
                // 情况1：兄弟是红色
                sibling->color = rb_color::black;
                node->parent->color = rb_color::red;
                left_rotate(node->parent);
                sibling = node->parent->right;
            }
            
            if ((!sibling->left || sibling->left->color == rb_color::black) &&
                (!sibling->right || sibling->right->color == rb_color::black)) {
                // 情况2：兄弟的两个孩子都是黑色
                sibling->color = rb_color::red;
                node = node->parent;
            } else {
                if (!sibling->right || sibling->right->color == rb_color::black) {
                    // 情况3：兄弟的左孩子是红色，右孩子是黑色
                    if (sibling->left) sibling->left->color = rb_color::black;
                    sibling->color = rb_color::red;
                    right_rotate(sibling);
                    sibling = node->parent->right;
                }
                
                // 情况4：兄弟的右孩子是红色
                sibling->color = node->parent->color;
                node->parent->color = rb_color::black;
                if (sibling->right) sibling->right->color = rb_color::black;
                left_rotate(node->parent);
                node = root_;
            }
        } else {
            // 对称情况
            node_pointer sibling = node->parent->left;
            if (sibling && sibling->color == rb_color::red) {
                sibling->color = rb_color::black;
                node->parent->color = rb_color::red;
                right_rotate(node->parent);
                sibling = node->parent->left;
            }
            
            if ((!sibling->left || sibling->left->color == rb_color::black) &&
                (!sibling->right || sibling->right->color == rb_color::black)) {
                sibling->color = rb_color::red;
                node = node->parent;
            } else {
                if (!sibling->left || sibling->left->color == rb_color::black) {
                    if (sibling->right) sibling->right->color = rb_color::black;
                    sibling->color = rb_color::red;
                    left_rotate(sibling);
                    sibling = node->parent->left;
                }
                
                sibling->color = node->parent->color;
                node->parent->color = rb_color::black;
                if (sibling->left) sibling->left->color = rb_color::black;
                right_rotate(node->parent);
                node = root_;
            }
        }
    }
    
    if (node) {
        node->color = rb_color::black;
    }
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
bool rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::is_valid_rb_tree() const {
    if (!root_) return true;
    
    // 性质2：根节点是黑色
    if (root_->color != rb_color::black) {
        return false;
    }
    
    // 检查每个节点
    auto check_node = [this](node_pointer node, auto&& self) -> std::pair<bool, int> {
        if (!node) return {true, 1};
        
        // 性质4：红色节点的子节点必须是黑色
        if (node->color == rb_color::red) {
            if ((node->left && node->left->color == rb_color::red) ||
                (node->right && node->right->color == rb_color::red)) {
                return {false, 0};
            }
        }
        
        auto [left_valid, left_black] = self(node->left, self);
        auto [right_valid, right_black] = self(node->right, self);
        
        if (!left_valid || !right_valid) {
            return {false, 0};
        }
        
        // 性质5：从每个节点到其所有后代叶子节点的路径包含相同数量的黑色节点
        if (left_black != right_black) {
            return {false, 0};
        }
        
        int black_height = left_black + (node->color == rb_color::black ? 1 : 0);
        return {true, black_height};
    };
    
    auto [valid, _] = check_node(root_, check_node);
    return valid;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
void rb_tree<Key, Value, KeyOfValue, Compare, Allocator>::verify_properties() const {
    if (!is_valid_rb_tree()) {
        throw std::runtime_error("Red-black tree properties violated");
    }
}

} // namespace stl

#endif // STL_RED_BLACK_TREE_HPP