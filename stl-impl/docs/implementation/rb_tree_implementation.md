# STL 红黑树实现文档

## 概述

本项目实现了一个完整的 STL 风格的红黑树（Red-Black Tree）数据结构。红黑树是一种自平衡二叉搜索树，通过特定的颜色和旋转操作来保持树的平衡，确保在最坏情况下基本操作的时间复杂度为 O(log n)。

## 设计特点

### 数据结构
- **节点结构**：每个节点包含数据、左右子节点指针、父节点指针和颜色标记
- **颜色标记**：使用枚举类型表示红色和黑色
- **双向迭代器**：支持前向和后向遍历
- **模板设计**：支持任意数据类型和自定义比较器

### 红黑树性质
1. **性质1**：每个节点要么是红色，要么是黑色
2. **性质2**：根节点是黑色
3. **性质3**：所有叶子节点（NIL）都是黑色
4. **性质4**：红色节点的两个子节点都是黑色
5. **性质5**：从任一节点到其每个叶子节点的所有路径都包含相同数量的黑色节点

### 时间复杂度
- **查找操作**：O(log n) - `find()`, `lower_bound()`, `upper_bound()`
- **插入操作**：O(log n) - `insert()`, `emplace()`
- **删除操作**：O(log n) - `erase()` (待实现)
- **遍历操作**：O(n) - 中序遍历
- **验证操作**：O(n) - `is_valid_rb_tree()`

### 空间复杂度
- **O(n)**：每个节点需要存储数据和指针信息
- **额外开销**：每个节点需要额外的颜色标记和父节点指针

## 核心功能实现

### 1. 节点结构
```cpp
template <typename T, typename Allocator>
struct rb_tree_node {
    T data;              // 节点数据
    node_pointer left;   // 左子节点
    node_pointer right;  // 右子节点
    node_pointer parent; // 父节点
    rb_color color;      // 节点颜色
};
```

### 2. 迭代器实现
- **双向迭代器**：支持 `++` 和 `--` 操作
- **常量迭代器**：提供只读访问
- **反向迭代器**：支持反向遍历

### 3. 基本操作
- **构造函数**：默认构造、拷贝构造、移动构造
- **赋值运算符**：拷贝赋值、移动赋值
- **容量操作**：`empty()`, `size()`, `max_size()`
- **修改器**：`clear()`, `insert()`, `emplace()`

### 4. 查找操作
- `find()`：精确查找
- `count()`：计数
- `lower_bound()`：下界查找
- `upper_bound()`：上界查找
- `equal_range()`：范围查找

### 5. 红黑树操作
- **insert_fixup()**：插入后的修复操作
- **left_rotate()**：左旋转
- **right_rotate()**：右旋转
- **transplant()**：节点替换

### 6. 验证功能
- `is_valid_rb_tree()`：验证红黑树性质
- `verify_properties()`：验证并抛出异常

## 模板参数

```cpp
template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator>
class rb_tree {
    // Key: 键类型
    // Value: 值类型
    // KeyOfValue: 从值中提取键的仿函数
    // Compare: 比较器
    // Allocator: 分配器
};
```

### 参数说明
- **Key**：用于比较的键类型
- **Value**：存储在树中的数据类型
- **KeyOfValue**：从 Value 中提取 Key 的仿函数
- **Compare**：键的比较器，默认为 `less<Key>`
- **Allocator**：内存分配器，默认为 `allocator<Value>`

## 红黑树操作详解

### 插入修复 (insert_fixup)

插入操作可能会破坏红黑树的性质，需要通过以下步骤修复：

1. **情况1**：叔叔节点是红色
   - 将父节点和叔叔节点设为黑色
   - 将祖父节点设为红色
   - 将当前节点移动到祖父节点

2. **情况2**：叔叔节点是黑色，当前节点是右孩子
   - 将当前节点移动到父节点
   - 进行左旋转

3. **情况3**：叔叔节点是黑色，当前节点是左孩子
   - 将父节点设为黑色
   - 将祖父节点设为红色
   - 对祖父节点进行右旋转

### 旋转操作

**左旋转 (left_rotate)**：
```
    x              y
   / \            / \
  a   y    =>    x   c
     / \        / \
    b   c      a   b
```

**右旋转 (right_rotate)**：
```
      y          x
     / \        / \
    x   c  =>  a   y
   / \            / \
  a   b          b   c
```

## 使用示例

### 基本使用
```cpp
#include "stl/tree/rb_tree.hpp"
#include "stl/functional.hpp"
#include <iostream>

using namespace stl;

struct Identity {
    template <typename T>
    const T& operator()(const T& x) const {
        return x;
    }
};

int main() {
    rb_tree<int, int, Identity, less<int>, allocator<int>> tree;
    
    // 插入元素
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(1);
    tree.insert(9);
    
    // 遍历元素（中序遍历，有序）
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;  // 输出: 1 3 5 7 9
    
    // 查找元素
    auto found = tree.find(5);
    if (found != tree.end()) {
        std::cout << "Found: " << *found << std::endl;
    }
    
    // 验证红黑树性质
    if (tree.is_valid_rb_tree()) {
        std::cout << "Valid red-black tree" << std::endl;
    }
    
    return 0;
}
```

### 使用 pair 作为值
```cpp
#include "stl/tree/rb_tree.hpp"
#include "stl/functional.hpp"
#include <iostream>

using namespace stl;

struct Select1st {
    template <typename T>
    const typename T::first_type& operator()(const T& x) const {
        return x.first;
    }
};

int main() {
    using Pair = std::pair<int, std::string>;
    rb_tree<int, Pair, Select1st, less<int>, allocator<Pair>> tree;
    
    // 插入键值对
    tree.insert({1, "one"});
    tree.insert({3, "three"});
    tree.insert({2, "two"});
    
    // 遍历（按键排序）
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    
    return 0;
}
```

### 使用自定义比较器
```cpp
#include "stl/tree/rb_tree.hpp"
#include "stl/functional.hpp"
#include <iostream>

using namespace stl;

struct Identity {
    template <typename T>
    const T& operator()(const T& x) const {
        return x;
    }
};

int main() {
    // 使用 greater<int> 实现降序排列
    rb_tree<int, int, Identity, greater<int>, allocator<int>> tree;
    
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    
    // 遍历（降序）
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;  // 输出: 7 5 3
    
    return 0;
}
```

## 测试覆盖

实现包含全面的测试用例，覆盖以下方面：

1. **基本操作**：构造、插入、查找、遍历
2. **容量操作**：empty、size、max_size
3. **拷贝语义**：拷贝构造、拷贝赋值
4. **移动语义**：移动构造、移动赋值
5. **迭代器操作**：正向迭代、反向迭代
6. **查找操作**：find、count、bound操作
7. **红黑树性质**：验证红黑树的五个性质
8. **大数据集**：大量数据的插入和查找
9. **字符串类型**：复杂数据类型支持
10. **自定义类型**：用户定义类型和比较器
11. **边界条件**：空树、单节点树等特殊情况

## 性能特征

### 优势
- **对数复杂度**：所有基本操作都是 O(log n)
- **自平衡**：自动维护树的平衡，避免最坏情况
- **有序性**：中序遍历得到有序序列
- **内存效率**：相比其他平衡树，内存开销相对较小

### 与其他平衡树的比较
1. **AVL树**：
   - 优点：更严格的平衡，查找更快
   - 缺点：插入删除需要更多旋转

2. **B树**：
   - 优点：减少磁盘I/O，适合数据库
   - 缺点：实现复杂，内存开销大

3. **红黑树**：
   - 优点：平衡插入和查找性能，实现相对简单
   - 缺点：不如AVL树严格平衡

## 应用场景

红黑树广泛应用于：

1. **STL关联容器**：`std::map`, `std::set` 的底层实现
2. **数据库索引**：B+树的节点内部使用
3. **操作系统**：进程调度、内存管理
4. **网络路由**：路由表的高效查找
5. **编译器**：符号表管理

## 实现细节

### 内存管理
- 使用分配器进行内存管理
- 支持自定义分配器
- 节点分配和释放的异常安全

### 异常安全
- 基本的异常安全保证
- 内存分配失败时不会泄漏资源
- 操作失败时保持数据一致性

### 迭代器失效
- 插入操作不会使迭代器失效
- 删除操作只会使被删除节点的迭代器失效

## 扩展方向

如果需要进一步提升这个实现，可以考虑：

1. **删除操作**：实现完整的删除功能
2. **更多操作**：合并、分割等高级操作
3. **性能优化**：缓存友好的内存布局
4. **并发支持**：线程安全的实现
5. **序列化**：支持持久化存储
6. **调试工具**：可视化树结构
7. **统计信息**：树高、平衡因子等指标

## 注意事项

1. **键的唯一性**：当前实现假设键是唯一的
2. **比较器要求**：比较器必须满足严格弱序关系
3. **内存使用**：每个节点有额外的指针和颜色开销
4. **递归深度**：对于极大树的递归操作需要考虑栈空间

## 总结

这个红黑树实现提供了一个完整的、高效的自平衡二叉搜索树，具有良好的教学价值和实用性。通过理解这个实现，可以更好地掌握：

- 平衡二叉搜索树的原理
- 红黑树的五个性质及其维护
- 旋转操作在平衡算法中的应用
- 模板编程和泛型设计
- 迭代器模式和STL风格接口
- 异常安全和资源管理

对于学习数据结构和算法的开发者来说，这是一个有价值的参考实现。