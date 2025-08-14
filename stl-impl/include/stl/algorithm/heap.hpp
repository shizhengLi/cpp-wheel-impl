#ifndef STL_HEAP_HPP
#define STL_HEAP_HPP

#include <utility>
#include <functional>
#include <iterator>
#include <type_traits>

namespace stl {

// 堆算法实现
namespace detail {

// 向上调整堆
template <typename RandomIt, typename Compare>
void push_heap(RandomIt first, RandomIt last, Compare comp) {
    if (last - first <= 1) return;
    
    auto child = last - 1;
    auto parent = first + ((child - first - 1) / 2);
    
    while (child > first && comp(*parent, *child)) {
        using std::swap;
        swap(*parent, *child);
        child = parent;
        parent = first + ((child - first - 1) / 2);
    }
}

// 向下调整堆
template <typename RandomIt, typename Compare>
void adjust_heap(RandomIt first, RandomIt last, RandomIt hole, Compare comp) {
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
        
        using std::swap;
        swap(*hole, *(first + child));
        hole = first + child;
        parent = child;
        child = parent * 2 + 1;
    }
}

} // namespace detail

// 构建堆
template <typename RandomIt, typename Compare>
void make_heap(RandomIt first, RandomIt last, Compare comp) {
    static_assert(std::is_same_v<typename std::iterator_traits<RandomIt>::iterator_category, 
                   std::random_access_iterator_tag>, 
                   "Heap algorithms require random access iterators");
    
    auto len = last - first;
    if (len <= 1) return;
    
    auto parent = (len - 2) / 2;
    while (true) {
        detail::adjust_heap(first, last, first + parent, comp);
        if (parent == 0) break;
        --parent;
    }
}

// 插入元素到堆
template <typename RandomIt, typename Compare>
void push_heap(RandomIt first, RandomIt last, Compare comp) {
    static_assert(std::is_same_v<typename std::iterator_traits<RandomIt>::iterator_category, 
                   std::random_access_iterator_tag>, 
                   "Heap algorithms require random access iterators");
    detail::push_heap(first, last, comp);
}

// 从堆中移除元素
template <typename RandomIt, typename Compare>
void pop_heap(RandomIt first, RandomIt last, Compare comp) {
    static_assert(std::is_same_v<typename std::iterator_traits<RandomIt>::iterator_category, 
                   std::random_access_iterator_tag>, 
                   "Heap algorithms require random access iterators");
    
    if (last - first <= 1) return;
    
    using std::swap;
    swap(*first, *(last - 1));
    detail::adjust_heap(first, last - 1, first, comp);
}

// 堆排序
template <typename RandomIt, typename Compare>
void sort_heap(RandomIt first, RandomIt last, Compare comp) {
    static_assert(std::is_same_v<typename std::iterator_traits<RandomIt>::iterator_category, 
                   std::random_access_iterator_tag>, 
                   "Heap algorithms require random access iterators");
    
    while (last - first > 1) {
        pop_heap(first, last, comp);
        --last;
    }
}

// 检查是否为堆
template <typename RandomIt, typename Compare>
bool is_heap(RandomIt first, RandomIt last, Compare comp) {
    static_assert(std::is_same_v<typename std::iterator_traits<RandomIt>::iterator_category, 
                   std::random_access_iterator_tag>, 
                   "Heap algorithms require random access iterators");
    
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

// 默认比较器的重载
template <typename RandomIt>
void make_heap(RandomIt first, RandomIt last) {
    make_heap(first, last, less<typename std::iterator_traits<RandomIt>::value_type>());
}

template <typename RandomIt>
void push_heap(RandomIt first, RandomIt last) {
    push_heap(first, last, less<typename std::iterator_traits<RandomIt>::value_type>());
}

template <typename RandomIt>
void pop_heap(RandomIt first, RandomIt last) {
    pop_heap(first, last, less<typename std::iterator_traits<RandomIt>::value_type>());
}

template <typename RandomIt>
void sort_heap(RandomIt first, RandomIt last) {
    sort_heap(first, last, less<typename std::iterator_traits<RandomIt>::value_type>());
}

template <typename RandomIt>
bool is_heap(RandomIt first, RandomIt last) {
    return is_heap(first, last, less<typename std::iterator_traits<RandomIt>::value_type>());
}

} // namespace stl

#endif // STL_HEAP_HPP