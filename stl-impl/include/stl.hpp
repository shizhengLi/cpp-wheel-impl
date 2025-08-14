#ifndef STL_IMPL_HPP
#define STL_IMPL_HPP

// STL实现的主要头文件

// 基础工具
#include "stl/allocator.hpp"
#include "stl/iterator.hpp"
#include "stl/functional.hpp"

// 容器
#include "stl/container/vector.hpp"
#include "stl/container/deque.hpp"
#include "stl/container/list.hpp"
#include "stl/container/set.hpp"
#include "stl/container/map.hpp"
#include "stl/container/unordered_set.hpp"
#include "stl/container/unordered_map.hpp"

// 容器适配器
#include "stl/adapter/stack.hpp"
#include "stl/adapter/queue.hpp"
#include "stl/adapter/priority_queue.hpp"

// 算法
#include "stl/algorithm/basic.hpp"
#include "stl/algorithm/search.hpp"
#include "stl/algorithm/sort.hpp"
#include "stl/algorithm/heap.hpp"

namespace stl {
    // STL实现的主命名空间
}

#endif // STL_IMPL_HPP