#ifndef STL_ALGORITHM_BASIC_HPP
#define STL_ALGORITHM_BASIC_HPP

#include "../iterator.hpp"
#include "../functional.hpp"

namespace stl {

// Simple pair implementation
template <typename T1, typename T2>
struct pair {
    T1 first;
    T2 second;
    
    pair() = default;
    pair(const T1& a, const T2& b) : first(a), second(b) {}
    pair(const pair&) = default;
    pair(pair&&) = default;
    
    pair& operator=(const pair&) = default;
    pair& operator=(pair&&) = default;
};

template <typename T1, typename T2>
pair<T1, T2> make_pair(T1 t, T2 u) {
    return pair<T1, T2>(t, u);
}

} // namespace stl

namespace stl {

// 基本算法实现

// copy算法
template <typename InputIterator, typename OutputIterator>
OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
    while (first != last) {
        *result = *first;
        ++first;
        ++result;
    }
    return result;
}

// copy_backward算法
template <typename BidirectionalIterator1, typename BidirectionalIterator2>
BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, 
                                    BidirectionalIterator2 result) {
    while (first != last) {
        --last;
        --result;
        *result = *last;
    }
    return result;
}

// fill算法
template <typename ForwardIterator, typename T>
void fill(ForwardIterator first, ForwardIterator last, const T& value) {
    while (first != last) {
        *first = value;
        ++first;
    }
}

// fill_n算法
template <typename OutputIterator, typename Size, typename T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
    while (n > 0) {
        *first = value;
        ++first;
        --n;
    }
    return first;
}

// swap算法
template <typename T>
void swap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

// swap_ranges算法
template <typename ForwardIterator1, typename ForwardIterator2>
ForwardIterator2 swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2) {
    while (first1 != last1) {
        stl::swap(*first1, *first2);
        ++first1;
        ++first2;
    }
    return first2;
}

// min算法
template <typename T>
const T& min(const T& a, const T& b) {
    return (b < a) ? b : a;
}

template <typename T, typename Compare>
const T& min(const T& a, const T& b, Compare comp) {
    return comp(b, a) ? b : a;
}

// max算法
template <typename T>
const T& max(const T& a, const T& b) {
    return (a < b) ? b : a;
}

template <typename T, typename Compare>
const T& max(const T& a, const T& b, Compare comp) {
    return comp(a, b) ? b : a;
}

// minmax算法
template <typename T>
stl::pair<T, T> minmax(const T& a, const T& b) {
    return (b < a) ? stl::pair<T, T>(b, a) : stl::pair<T, T>(a, b);
}

template <typename T, typename Compare>
stl::pair<T, T> minmax(const T& a, const T& b, Compare comp) {
    return comp(b, a) ? stl::pair<T, T>(b, a) : stl::pair<T, T>(a, b);
}

// min_element算法
template <typename ForwardIterator>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last) {
    if (first == last) return last;
    
    ForwardIterator result = first;
    ++first;
    while (first != last) {
        if (*first < *result) {
            result = first;
        }
        ++first;
    }
    return result;
}

template <typename ForwardIterator, typename Compare>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last, Compare comp) {
    if (first == last) return last;
    
    ForwardIterator result = first;
    ++first;
    while (first != last) {
        if (comp(*first, *result)) {
            result = first;
        }
        ++first;
    }
    return result;
}

// max_element算法
template <typename ForwardIterator>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last) {
    if (first == last) return last;
    
    ForwardIterator result = first;
    ++first;
    while (first != last) {
        if (*result < *first) {
            result = first;
        }
        ++first;
    }
    return result;
}

template <typename ForwardIterator, typename Compare>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last, Compare comp) {
    if (first == last) return last;
    
    ForwardIterator result = first;
    ++first;
    while (first != last) {
        if (comp(*result, *first)) {
            result = first;
        }
        ++first;
    }
    return result;
}

// minmax_element算法
template <typename ForwardIterator>
stl::pair<ForwardIterator, ForwardIterator> minmax_element(ForwardIterator first, ForwardIterator last) {
    if (first == last) {
        return stl::pair<ForwardIterator, ForwardIterator>(last, last);
    }
    
    ForwardIterator min_result = first;
    ForwardIterator max_result = first;
    ++first;
    
    while (first != last) {
        if (*first < *min_result) {
            min_result = first;
        }
        if (*max_result < *first) {
            max_result = first;
        }
        ++first;
    }
    
    return stl::pair<ForwardIterator, ForwardIterator>(min_result, max_result);
}

template <typename ForwardIterator, typename Compare>
stl::pair<ForwardIterator, ForwardIterator> minmax_element(ForwardIterator first, ForwardIterator last, Compare comp) {
    if (first == last) {
        return stl::pair<ForwardIterator, ForwardIterator>(last, last);
    }
    
    ForwardIterator min_result = first;
    ForwardIterator max_result = first;
    ++first;
    
    while (first != last) {
        if (comp(*first, *min_result)) {
            min_result = first;
        }
        if (comp(*max_result, *first)) {
            max_result = first;
        }
        ++first;
    }
    
    return stl::pair<ForwardIterator, ForwardIterator>(min_result, max_result);
}

// equal算法
template <typename InputIterator1, typename InputIterator2>
bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
    while (first1 != last1) {
        if (*first1 != *first2) {
            return false;
        }
        ++first1;
        ++first2;
    }
    return true;
}

template <typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate pred) {
    while (first1 != last1) {
        if (!pred(*first1, *first2)) {
            return false;
        }
        ++first1;
        ++first2;
    }
    return true;
}

// lexicographical_compare算法
template <typename InputIterator1, typename InputIterator2>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2, InputIterator2 last2) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            return true;
        }
        if (*first2 < *first1) {
            return false;
        }
        ++first1;
        ++first2;
    }
    return (first1 == last1) && (first2 != last2);
}

template <typename InputIterator1, typename InputIterator2, typename Compare>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2, InputIterator2 last2, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            return true;
        }
        if (comp(*first2, *first1)) {
            return false;
        }
        ++first1;
        ++first2;
    }
    return (first1 == last1) && (first2 != last2);
}

} // namespace stl

#endif // STL_ALGORITHM_BASIC_HPP