#ifndef STL_ALGORITHM_SORT_HPP
#define STL_ALGORITHM_SORT_HPP

#include "../iterator.hpp"
#include "../functional.hpp"
#include "basic.hpp"
#include <utility>

namespace stl {

// 排序算法实现

// 辅助函数：插入排序
template <typename RandomAccessIterator, typename Compare>
void insertion_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    if (first == last) return;
    
    for (RandomAccessIterator i = first + 1; i != last; ++i) {
        typename stl::iterator_traits<RandomAccessIterator>::value_type value = *i;
        RandomAccessIterator j = i;
        
        while (j != first && comp(value, *(j - 1))) {
            *j = *(j - 1);
            --j;
        }
        *j = value;
    }
}

// 辅助函数：快速排序的分区
template <typename RandomAccessIterator, typename Compare>
RandomAccessIterator quick_sort_partition(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    typename stl::iterator_traits<RandomAccessIterator>::value_type pivot = *(last - 1);
    RandomAccessIterator i = first;
    
    for (RandomAccessIterator j = first; j != last - 1; ++j) {
        if (comp(*j, pivot)) {
            stl::swap(*i, *j);
            ++i;
        }
    }
    stl::swap(*i, *(last - 1));
    return i;
}

// 辅助函数：快速排序
template <typename RandomAccessIterator, typename Compare>
void quick_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    if (last - first <= 16) {
        insertion_sort(first, last, comp);
        return;
    }
    
    RandomAccessIterator pivot = quick_sort_partition(first, last, comp);
    quick_sort(first, pivot, comp);
    quick_sort(pivot + 1, last, comp);
}

// sort算法
template <typename RandomAccessIterator>
void sort(RandomAccessIterator first, RandomAccessIterator last) {
    sort_impl(first, last, stl::less<typename stl::iterator_traits<RandomAccessIterator>::value_type>());
}

// sort implementation (to avoid name conflicts)
template <typename RandomAccessIterator, typename Compare>
void sort_impl(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    if (first == last) return;
    quick_sort(first, last, comp);
}

template <typename RandomAccessIterator, typename Compare>
void sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    sort_impl(first, last, comp);
}

// stable_sort算法 (简化实现，使用插入排序保证稳定性)
template <typename RandomAccessIterator>
void stable_sort(RandomAccessIterator first, RandomAccessIterator last) {
    stable_sort_impl(first, last, stl::less<typename stl::iterator_traits<RandomAccessIterator>::value_type>());
}

// stable_sort implementation (to avoid name conflicts)
template <typename RandomAccessIterator, typename Compare>
void stable_sort_impl(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    if (first == last) return;
    insertion_sort(first, last, comp);
}

template <typename RandomAccessIterator, typename Compare>
void stable_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    stable_sort_impl(first, last, comp);
}

// partial_sort算法
template <typename RandomAccessIterator>
void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last) {
    partial_sort_impl(first, middle, last, stl::less<typename stl::iterator_traits<RandomAccessIterator>::value_type>());
}

// partial_sort implementation (to avoid name conflicts)
template <typename RandomAccessIterator, typename Compare>
void partial_sort_impl(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, Compare comp) {
    if (first == middle || middle == last) return;
    
    // 简化实现：对[first, middle)进行排序
    sort_impl(first, middle, comp);
    
    // 将middle到last的元素与middle-1比较，如果更小则交换并重新排序
    for (RandomAccessIterator i = middle; i != last; ++i) {
        if (comp(*i, *(middle - 1))) {
            stl::swap(*i, *(middle - 1));
            // 重新排序[first, middle)
            RandomAccessIterator j = middle - 1;
            while (j != first && comp(*j, *(j - 1))) {
                stl::swap(*j, *(j - 1));
                --j;
            }
        }
    }
}

template <typename RandomAccessIterator, typename Compare>
void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, Compare comp) {
    partial_sort_impl(first, middle, last, comp);
}

// partition算法
template <typename ForwardIterator, typename Predicate>
ForwardIterator partition(ForwardIterator first, ForwardIterator last, Predicate pred) {
    first = stl::find_if_not(first, last, pred);
    if (first == last) return first;
    
    for (ForwardIterator it = first + 1; it != last; ++it) {
        if (pred(*it)) {
            stl::swap(*first, *it);
            ++first;
        }
    }
    return first;
}

// stable_partition算法 (简化实现)
template <typename ForwardIterator, typename Predicate>
ForwardIterator stable_partition(ForwardIterator first, ForwardIterator last, Predicate pred) {
    // 简化实现：使用临时存储
    ForwardIterator result = first;
    for (ForwardIterator it = first; it != last; ++it) {
        if (pred(*it)) {
            if (result != it) {
                typename stl::iterator_traits<ForwardIterator>::value_type temp = *it;
                ForwardIterator shift = it;
                while (shift != result) {
                    *shift = *(shift - 1);
                    --shift;
                }
                *result = temp;
            }
            ++result;
        }
    }
    return result;
}

// nth_element算法
template <typename RandomAccessIterator>
void nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last) {
    nth_element_impl(first, nth, last, stl::less<typename stl::iterator_traits<RandomAccessIterator>::value_type>());
}

// nth_element implementation (to avoid name conflicts)
template <typename RandomAccessIterator, typename Compare>
void nth_element_impl(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last, Compare comp) {
    if (first == last || nth == last) return;
    
    while (true) {
        if (last - first <= 1) return;
        
        // Use a simple pivot selection and partition
        RandomAccessIterator pivot = first;
        RandomAccessIterator i = first + 1;
        RandomAccessIterator j = last - 1;
        
        while (i <= j) {
            while (i <= j && comp(*i, *pivot)) ++i;
            while (i <= j && comp(*pivot, *j)) --j;
            if (i <= j) {
                stl::swap(*i, *j);
                ++i;
                --j;
            }
        }
        stl::swap(*pivot, *j);
        pivot = j;
        
        if (nth < pivot) {
            last = pivot;
        } else if (nth > pivot) {
            first = pivot + 1;
        } else {
            return;
        }
    }
}

template <typename RandomAccessIterator, typename Compare>
void nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last, Compare comp) {
    nth_element_impl(first, nth, last, comp);
}

// is_sorted算法
template <typename ForwardIterator>
bool is_sorted(ForwardIterator first, ForwardIterator last) {
    return is_sorted_impl(first, last, stl::less<typename stl::iterator_traits<ForwardIterator>::value_type>());
}

// is_sorted implementation (to avoid name conflicts)
template <typename ForwardIterator, typename Compare>
bool is_sorted_impl(ForwardIterator first, ForwardIterator last, Compare comp) {
    if (first == last) return true;
    
    ForwardIterator next = first;
    ++next;
    while (next != last) {
        if (comp(*next, *first)) {
            return false;
        }
        first = next;
        ++next;
    }
    return true;
}

template <typename ForwardIterator, typename Compare>
bool is_sorted(ForwardIterator first, ForwardIterator last, Compare comp) {
    return is_sorted_impl(first, last, comp);
}

// is_sorted_until算法
template <typename ForwardIterator>
ForwardIterator is_sorted_until(ForwardIterator first, ForwardIterator last) {
    return is_sorted_until_impl(first, last, stl::less<typename stl::iterator_traits<ForwardIterator>::value_type>());
}

// is_sorted_until implementation (to avoid name conflicts)
template <typename ForwardIterator, typename Compare>
ForwardIterator is_sorted_until_impl(ForwardIterator first, ForwardIterator last, Compare comp) {
    if (first == last) return last;
    
    ForwardIterator next = first;
    ++next;
    while (next != last) {
        if (comp(*next, *first)) {
            return next;
        }
        first = next;
        ++next;
    }
    return last;
}

template <typename ForwardIterator, typename Compare>
ForwardIterator is_sorted_until(ForwardIterator first, ForwardIterator last, Compare comp) {
    return is_sorted_until_impl(first, last, comp);
}

} // namespace stl

#endif // STL_ALGORITHM_SORT_HPP