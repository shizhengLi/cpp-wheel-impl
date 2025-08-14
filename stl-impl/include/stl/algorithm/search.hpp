#ifndef STL_ALGORITHM_SEARCH_HPP
#define STL_ALGORITHM_SEARCH_HPP

#include "../iterator.hpp"
#include "../functional.hpp"
#include "basic.hpp"

namespace stl {

// 查找算法实现

// find算法
template <typename InputIterator, typename T>
InputIterator find(InputIterator first, InputIterator last, const T& value) {
    while (first != last) {
        if (*first == value) {
            return first;
        }
        ++first;
    }
    return last;
}

// find_if算法
template <typename InputIterator, typename Predicate>
InputIterator find_if(InputIterator first, InputIterator last, Predicate pred) {
    while (first != last) {
        if (pred(*first)) {
            return first;
        }
        ++first;
    }
    return last;
}

// find_if_not算法
template <typename InputIterator, typename Predicate>
InputIterator find_if_not(InputIterator first, InputIterator last, Predicate pred) {
    while (first != last) {
        if (!pred(*first)) {
            return first;
        }
        ++first;
    }
    return last;
}

// search算法
template <typename ForwardIterator1, typename ForwardIterator2>
ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
                       ForwardIterator2 first2, ForwardIterator2 last2) {
    if (first2 == last2) {
        return first1;
    }
    
    while (first1 != last1) {
        ForwardIterator1 it1 = first1;
        ForwardIterator2 it2 = first2;
        
        while (*it1 == *it2) {
            ++it1;
            ++it2;
            if (it2 == last2) {
                return first1;
            }
            if (it1 == last1) {
                return last1;
            }
        }
        ++first1;
    }
    return last1;
}

template <typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
                       ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate pred) {
    if (first2 == last2) {
        return first1;
    }
    
    while (first1 != last1) {
        ForwardIterator1 it1 = first1;
        ForwardIterator2 it2 = first2;
        
        while (pred(*it1, *it2)) {
            ++it1;
            ++it2;
            if (it2 == last2) {
                return first1;
            }
            if (it1 == last1) {
                return last1;
            }
        }
        ++first1;
    }
    return last1;
}

// find_end算法
template <typename ForwardIterator1, typename ForwardIterator2>
ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                         ForwardIterator2 first2, ForwardIterator2 last2) {
    if (first2 == last2) {
        return last1;
    }
    
    ForwardIterator1 result = last1;
    while (true) {
        ForwardIterator1 new_result = stl::search(first1, last1, first2, last2);
        if (new_result == last1) {
            break;
        }
        result = new_result;
        first1 = new_result;
        ++first1;
    }
    return result;
}

template <typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                         ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate pred) {
    if (first2 == last2) {
        return last1;
    }
    
    ForwardIterator1 result = last1;
    while (true) {
        ForwardIterator1 new_result = stl::search(first1, last1, first2, last2, pred);
        if (new_result == last1) {
            break;
        }
        result = new_result;
        first1 = new_result;
        ++first1;
    }
    return result;
}

// find_first_of算法
template <typename InputIterator, typename ForwardIterator>
InputIterator find_first_of(InputIterator first1, InputIterator last1,
                           ForwardIterator first2, ForwardIterator last2) {
    while (first1 != last1) {
        for (ForwardIterator it = first2; it != last2; ++it) {
            if (*first1 == *it) {
                return first1;
            }
        }
        ++first1;
    }
    return last1;
}

template <typename InputIterator, typename ForwardIterator, typename BinaryPredicate>
InputIterator find_first_of(InputIterator first1, InputIterator last1,
                           ForwardIterator first2, ForwardIterator last2, BinaryPredicate pred) {
    while (first1 != last1) {
        for (ForwardIterator it = first2; it != last2; ++it) {
            if (pred(*first1, *it)) {
                return first1;
            }
        }
        ++first1;
    }
    return last1;
}

// adjacent_find算法
template <typename ForwardIterator>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last) {
    if (first == last) {
        return last;
    }
    
    ForwardIterator next = first;
    ++next;
    while (next != last) {
        if (*first == *next) {
            return first;
        }
        first = next;
        ++next;
    }
    return last;
}

template <typename ForwardIterator, typename BinaryPredicate>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last, BinaryPredicate pred) {
    if (first == last) {
        return last;
    }
    
    ForwardIterator next = first;
    ++next;
    while (next != last) {
        if (pred(*first, *next)) {
            return first;
        }
        first = next;
        ++next;
    }
    return last;
}

// count算法
template <typename InputIterator, typename T>
typename stl::iterator_traits<InputIterator>::difference_type
count(InputIterator first, InputIterator last, const T& value) {
    typename stl::iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        if (*first == value) {
            ++n;
        }
        ++first;
    }
    return n;
}

// count_if算法
template <typename InputIterator, typename Predicate>
typename stl::iterator_traits<InputIterator>::difference_type
count_if(InputIterator first, InputIterator last, Predicate pred) {
    typename stl::iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        if (pred(*first)) {
            ++n;
        }
        ++first;
    }
    return n;
}

// search_n算法
template <typename ForwardIterator, typename Size, typename T>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Size count, const T& value) {
    if (count == 0) {
        return first;
    }
    
    Size remaining = count;
    ForwardIterator result = first;
    
    while (first != last) {
        if (*first == value) {
            if (remaining == count) {
                result = first;
            }
            --remaining;
            if (remaining == 0) {
                return result;
            }
        } else {
            remaining = count;
        }
        ++first;
    }
    return last;
}

template <typename ForwardIterator, typename Size, typename T, typename BinaryPredicate>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Size count, const T& value, BinaryPredicate pred) {
    if (count == 0) {
        return first;
    }
    
    Size remaining = count;
    ForwardIterator result = first;
    
    while (first != last) {
        if (pred(*first, value)) {
            if (remaining == count) {
                result = first;
            }
            --remaining;
            if (remaining == 0) {
                return result;
            }
        } else {
            remaining = count;
        }
        ++first;
    }
    return last;
}

// 二分查找算法 (要求序列已排序)

// lower_bound算法
template <typename ForwardIterator, typename T>
ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value) {
    ForwardIterator it;
    typename stl::iterator_traits<ForwardIterator>::difference_type count, step;
    count = stl::distance(first, last);
    
    while (count > 0) {
        it = first;
        step = count / 2;
        stl::advance(it, step);
        if (*it < value) {
            first = ++it;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

template <typename ForwardIterator, typename T, typename Compare>
ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
    ForwardIterator it;
    typename stl::iterator_traits<ForwardIterator>::difference_type count, step;
    count = stl::distance(first, last);
    
    while (count > 0) {
        it = first;
        step = count / 2;
        stl::advance(it, step);
        if (comp(*it, value)) {
            first = ++it;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

// upper_bound算法
template <typename ForwardIterator, typename T>
ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value) {
    ForwardIterator it;
    typename stl::iterator_traits<ForwardIterator>::difference_type count, step;
    count = stl::distance(first, last);
    
    while (count > 0) {
        it = first;
        step = count / 2;
        stl::advance(it, step);
        if (!(value < *it)) {
            first = ++it;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

template <typename ForwardIterator, typename T, typename Compare>
ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
    ForwardIterator it;
    typename stl::iterator_traits<ForwardIterator>::difference_type count, step;
    count = stl::distance(first, last);
    
    while (count > 0) {
        it = first;
        step = count / 2;
        stl::advance(it, step);
        if (!comp(value, *it)) {
            first = ++it;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

// binary_search算法
template <typename ForwardIterator, typename T>
bool binary_search(ForwardIterator first, ForwardIterator last, const T& value) {
    first = stl::lower_bound(first, last, value);
    return (first != last && !(value < *first));
}

template <typename ForwardIterator, typename T, typename Compare>
bool binary_search(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
    first = stl::lower_bound(first, last, value, comp);
    return (first != last && !comp(value, *first));
}

// equal_range算法
template <typename ForwardIterator, typename T>
stl::pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T& value) {
    ForwardIterator lower = stl::lower_bound(first, last, value);
    ForwardIterator upper = stl::upper_bound(lower, last, value);
    return stl::pair<ForwardIterator, ForwardIterator>(lower, upper);
}

template <typename ForwardIterator, typename T, typename Compare>
stl::pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
    ForwardIterator lower = stl::lower_bound(first, last, value, comp);
    ForwardIterator upper = stl::upper_bound(lower, last, value, comp);
    return stl::pair<ForwardIterator, ForwardIterator>(lower, upper);
}

} // namespace stl

#endif // STL_ALGORITHM_SEARCH_HPP