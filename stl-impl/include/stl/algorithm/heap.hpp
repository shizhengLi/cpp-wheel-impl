#ifndef STL_ALGORITHM_HEAP_HPP
#define STL_ALGORITHM_HEAP_HPP

#include "../iterator.hpp"
#include "../functional.hpp"
#include "basic.hpp"

namespace stl {

// 堆算法实现

// 辅助函数：上浮
template <typename RandomAccessIterator, typename Compare>
void push_heap_up(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    if (last - first <= 1) return;
    
    auto child = last - 1;
    auto parent = first + ((child - first - 1) / 2);
    
    while (child > first && comp(*parent, *child)) {
        stl::swap(*parent, *child);
        child = parent;
        parent = first + ((child - first - 1) / 2);
    }
}

// 辅助函数：下沉
template <typename RandomAccessIterator, typename Compare>
void push_heap_down(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator hole, Compare comp) {
    auto len = last - first;
    auto parent = hole - first;
    auto child = parent * 2 + 1;
    
    while (child < len) {
        if (child + 1 < len && comp(*(first + child), *(first + child + 1))) {
            ++child;
        }
        
        if (!comp(*hole, *(first + child))) {
            break;
        }
        
        stl::swap(*hole, *(first + child));
        hole = first + child;
        parent = child;
        child = parent * 2 + 1;
    }
}

// make_heap算法
template <typename RandomAccessIterator>
void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
    make_heap_impl(first, last, stl::less<typename stl::iterator_traits<RandomAccessIterator>::value_type>());
}

// make_heap implementation (to avoid name conflicts)
template <typename RandomAccessIterator, typename Compare>
void make_heap_impl(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    auto len = last - first;
    if (len <= 1) return;
    
    auto parent = (len - 2) / 2;
    while (true) {
        push_heap_down(first, last, first + parent, comp);
        if (parent == 0) break;
        --parent;
    }
}

template <typename RandomAccessIterator, typename Compare>
void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    make_heap_impl(first, last, comp);
}

// push_heap算法
template <typename RandomAccessIterator>
void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
    push_heap_impl(first, last, stl::less<typename stl::iterator_traits<RandomAccessIterator>::value_type>());
}

// push_heap implementation (to avoid name conflicts)
template <typename RandomAccessIterator, typename Compare>
void push_heap_impl(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    push_heap_up(first, last, comp);
}

template <typename RandomAccessIterator, typename Compare>
void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    push_heap_impl(first, last, comp);
}

// pop_heap算法
template <typename RandomAccessIterator>
void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
    pop_heap_impl(first, last, stl::less<typename stl::iterator_traits<RandomAccessIterator>::value_type>());
}

// pop_heap implementation (to avoid name conflicts)
template <typename RandomAccessIterator, typename Compare>
void pop_heap_impl(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    if (last - first <= 1) return;
    
    stl::swap(*first, *(last - 1));
    push_heap_down(first, last - 1, first, comp);
}

template <typename RandomAccessIterator, typename Compare>
void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    pop_heap_impl(first, last, comp);
}

// sort_heap算法
template <typename RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
    sort_heap_impl(first, last, stl::less<typename stl::iterator_traits<RandomAccessIterator>::value_type>());
}

// sort_heap implementation (to avoid name conflicts)
template <typename RandomAccessIterator, typename Compare>
void sort_heap_impl(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    while (last - first > 1) {
        pop_heap_impl(first, last, comp);
        --last;
    }
}

template <typename RandomAccessIterator, typename Compare>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    sort_heap_impl(first, last, comp);
}

// is_heap算法
template <typename RandomAccessIterator>
bool is_heap(RandomAccessIterator first, RandomAccessIterator last) {
    return is_heap_impl(first, last, stl::less<typename stl::iterator_traits<RandomAccessIterator>::value_type>());
}

// is_heap implementation (to avoid name conflicts)
template <typename RandomAccessIterator, typename Compare>
bool is_heap_impl(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    auto len = last - first;
    for (auto parent = 0; parent < len; ++parent) {
        auto left = parent * 2 + 1;
        auto right = parent * 2 + 2;
        
        if (left < len && comp(*(first + parent), *(first + left))) {
            return false;
        }
        
        if (right < len && comp(*(first + parent), *(first + right))) {
            return false;
        }
    }
    return true;
}

template <typename RandomAccessIterator, typename Compare>
bool is_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    return is_heap_impl(first, last, comp);
}

// is_heap_until算法
template <typename RandomAccessIterator>
RandomAccessIterator is_heap_until(RandomAccessIterator first, RandomAccessIterator last) {
    return is_heap_until_impl(first, last, stl::less<typename stl::iterator_traits<RandomAccessIterator>::value_type>());
}

// is_heap_until implementation (to avoid name conflicts)
template <typename RandomAccessIterator, typename Compare>
RandomAccessIterator is_heap_until_impl(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    auto len = last - first;
    for (auto parent = 0; parent < len; ++parent) {
        auto left = parent * 2 + 1;
        auto right = parent * 2 + 2;
        
        if (left < len && comp(*(first + parent), *(first + left))) {
            return first + left;
        }
        
        if (right < len && comp(*(first + parent), *(first + right))) {
            return first + right;
        }
    }
    return last;
}

template <typename RandomAccessIterator, typename Compare>
RandomAccessIterator is_heap_until(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    return is_heap_until_impl(first, last, comp);
}

} // namespace stl

#endif // STL_ALGORITHM_HEAP_HPP